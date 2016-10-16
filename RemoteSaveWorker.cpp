#include "RemoteSaveWorker.h"
#include <arpa/inet.h>
#include "Document.h"
#include "PreferenceDialog.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1440

unsigned char RemoteSaveWorker::aesGfMul[256][6] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, {0x02, 0x03, 0x09, 0x0b, 0x0d, 0x0e},
    {0x04, 0x06, 0x12, 0x16, 0x1a, 0x1c}, {0x06, 0x05, 0x1b, 0x1d, 0x17, 0x12},
    {0x08, 0x0c, 0x24, 0x2c, 0x34, 0x38}, {0x0a, 0x0f, 0x2d, 0x27, 0x39, 0x36},
    {0x0c, 0x0a, 0x36, 0x3a, 0x2e, 0x24}, {0x0e, 0x09, 0x3f, 0x31, 0x23, 0x2a},
    {0x10, 0x18, 0x48, 0x58, 0x68, 0x70}, {0x12, 0x1b, 0x41, 0x53, 0x65, 0x7e},
    {0x14, 0x1e, 0x5a, 0x4e, 0x72, 0x6c}, {0x16, 0x1d, 0x53, 0x45, 0x7f, 0x62},
    {0x18, 0x14, 0x6c, 0x74, 0x5c, 0x48}, {0x1a, 0x17, 0x65, 0x7f, 0x51, 0x46},
    {0x1c, 0x12, 0x7e, 0x62, 0x46, 0x54}, {0x1e, 0x11, 0x77, 0x69, 0x4b, 0x5a},
    {0x20, 0x30, 0x90, 0xb0, 0xd0, 0xe0}, {0x22, 0x33, 0x99, 0xbb, 0xdd, 0xee},
    {0x24, 0x36, 0x82, 0xa6, 0xca, 0xfc}, {0x26, 0x35, 0x8b, 0xad, 0xc7, 0xf2},
    {0x28, 0x3c, 0xb4, 0x9c, 0xe4, 0xd8}, {0x2a, 0x3f, 0xbd, 0x97, 0xe9, 0xd6},
    {0x2c, 0x3a, 0xa6, 0x8a, 0xfe, 0xc4}, {0x2e, 0x39, 0xaf, 0x81, 0xf3, 0xca},
    {0x30, 0x28, 0xd8, 0xe8, 0xb8, 0x90}, {0x32, 0x2b, 0xd1, 0xe3, 0xb5, 0x9e},
    {0x34, 0x2e, 0xca, 0xfe, 0xa2, 0x8c}, {0x36, 0x2d, 0xc3, 0xf5, 0xaf, 0x82},
    {0x38, 0x24, 0xfc, 0xc4, 0x8c, 0xa8}, {0x3a, 0x27, 0xf5, 0xcf, 0x81, 0xa6},
    {0x3c, 0x22, 0xee, 0xd2, 0x96, 0xb4}, {0x3e, 0x21, 0xe7, 0xd9, 0x9b, 0xba},
    {0x40, 0x60, 0x3b, 0x7b, 0xbb, 0xdb}, {0x42, 0x63, 0x32, 0x70, 0xb6, 0xd5},
    {0x44, 0x66, 0x29, 0x6d, 0xa1, 0xc7}, {0x46, 0x65, 0x20, 0x66, 0xac, 0xc9},
    {0x48, 0x6c, 0x1f, 0x57, 0x8f, 0xe3}, {0x4a, 0x6f, 0x16, 0x5c, 0x82, 0xed},
    {0x4c, 0x6a, 0x0d, 0x41, 0x95, 0xff}, {0x4e, 0x69, 0x04, 0x4a, 0x98, 0xf1},
    {0x50, 0x78, 0x73, 0x23, 0xd3, 0xab}, {0x52, 0x7b, 0x7a, 0x28, 0xde, 0xa5},
    {0x54, 0x7e, 0x61, 0x35, 0xc9, 0xb7}, {0x56, 0x7d, 0x68, 0x3e, 0xc4, 0xb9},
    {0x58, 0x74, 0x57, 0x0f, 0xe7, 0x93}, {0x5a, 0x77, 0x5e, 0x04, 0xea, 0x9d},
    {0x5c, 0x72, 0x45, 0x19, 0xfd, 0x8f}, {0x5e, 0x71, 0x4c, 0x12, 0xf0, 0x81},
    {0x60, 0x50, 0xab, 0xcb, 0x6b, 0x3b}, {0x62, 0x53, 0xa2, 0xc0, 0x66, 0x35},
    {0x64, 0x56, 0xb9, 0xdd, 0x71, 0x27}, {0x66, 0x55, 0xb0, 0xd6, 0x7c, 0x29},
    {0x68, 0x5c, 0x8f, 0xe7, 0x5f, 0x03}, {0x6a, 0x5f, 0x86, 0xec, 0x52, 0x0d},
    {0x6c, 0x5a, 0x9d, 0xf1, 0x45, 0x1f}, {0x6e, 0x59, 0x94, 0xfa, 0x48, 0x11},
    {0x70, 0x48, 0xe3, 0x93, 0x03, 0x4b}, {0x72, 0x4b, 0xea, 0x98, 0x0e, 0x45},
    {0x74, 0x4e, 0xf1, 0x85, 0x19, 0x57}, {0x76, 0x4d, 0xf8, 0x8e, 0x14, 0x59},
    {0x78, 0x44, 0xc7, 0xbf, 0x37, 0x73}, {0x7a, 0x47, 0xce, 0xb4, 0x3a, 0x7d},
    {0x7c, 0x42, 0xd5, 0xa9, 0x2d, 0x6f}, {0x7e, 0x41, 0xdc, 0xa2, 0x20, 0x61},
    {0x80, 0xc0, 0x76, 0xf6, 0x6d, 0xad}, {0x82, 0xc3, 0x7f, 0xfd, 0x60, 0xa3},
    {0x84, 0xc6, 0x64, 0xe0, 0x77, 0xb1}, {0x86, 0xc5, 0x6d, 0xeb, 0x7a, 0xbf},
    {0x88, 0xcc, 0x52, 0xda, 0x59, 0x95}, {0x8a, 0xcf, 0x5b, 0xd1, 0x54, 0x9b},
    {0x8c, 0xca, 0x40, 0xcc, 0x43, 0x89}, {0x8e, 0xc9, 0x49, 0xc7, 0x4e, 0x87},
    {0x90, 0xd8, 0x3e, 0xae, 0x05, 0xdd}, {0x92, 0xdb, 0x37, 0xa5, 0x08, 0xd3},
    {0x94, 0xde, 0x2c, 0xb8, 0x1f, 0xc1}, {0x96, 0xdd, 0x25, 0xb3, 0x12, 0xcf},
    {0x98, 0xd4, 0x1a, 0x82, 0x31, 0xe5}, {0x9a, 0xd7, 0x13, 0x89, 0x3c, 0xeb},
    {0x9c, 0xd2, 0x08, 0x94, 0x2b, 0xf9}, {0x9e, 0xd1, 0x01, 0x9f, 0x26, 0xf7},
    {0xa0, 0xf0, 0xe6, 0x46, 0xbd, 0x4d}, {0xa2, 0xf3, 0xef, 0x4d, 0xb0, 0x43},
    {0xa4, 0xf6, 0xf4, 0x50, 0xa7, 0x51}, {0xa6, 0xf5, 0xfd, 0x5b, 0xaa, 0x5f},
    {0xa8, 0xfc, 0xc2, 0x6a, 0x89, 0x75}, {0xaa, 0xff, 0xcb, 0x61, 0x84, 0x7b},
    {0xac, 0xfa, 0xd0, 0x7c, 0x93, 0x69}, {0xae, 0xf9, 0xd9, 0x77, 0x9e, 0x67},
    {0xb0, 0xe8, 0xae, 0x1e, 0xd5, 0x3d}, {0xb2, 0xeb, 0xa7, 0x15, 0xd8, 0x33},
    {0xb4, 0xee, 0xbc, 0x08, 0xcf, 0x21}, {0xb6, 0xed, 0xb5, 0x03, 0xc2, 0x2f},
    {0xb8, 0xe4, 0x8a, 0x32, 0xe1, 0x05}, {0xba, 0xe7, 0x83, 0x39, 0xec, 0x0b},
    {0xbc, 0xe2, 0x98, 0x24, 0xfb, 0x19}, {0xbe, 0xe1, 0x91, 0x2f, 0xf6, 0x17},
    {0xc0, 0xa0, 0x4d, 0x8d, 0xd6, 0x76}, {0xc2, 0xa3, 0x44, 0x86, 0xdb, 0x78},
    {0xc4, 0xa6, 0x5f, 0x9b, 0xcc, 0x6a}, {0xc6, 0xa5, 0x56, 0x90, 0xc1, 0x64},
    {0xc8, 0xac, 0x69, 0xa1, 0xe2, 0x4e}, {0xca, 0xaf, 0x60, 0xaa, 0xef, 0x40},
    {0xcc, 0xaa, 0x7b, 0xb7, 0xf8, 0x52}, {0xce, 0xa9, 0x72, 0xbc, 0xf5, 0x5c},
    {0xd0, 0xb8, 0x05, 0xd5, 0xbe, 0x06}, {0xd2, 0xbb, 0x0c, 0xde, 0xb3, 0x08},
    {0xd4, 0xbe, 0x17, 0xc3, 0xa4, 0x1a}, {0xd6, 0xbd, 0x1e, 0xc8, 0xa9, 0x14},
    {0xd8, 0xb4, 0x21, 0xf9, 0x8a, 0x3e}, {0xda, 0xb7, 0x28, 0xf2, 0x87, 0x30},
    {0xdc, 0xb2, 0x33, 0xef, 0x90, 0x22}, {0xde, 0xb1, 0x3a, 0xe4, 0x9d, 0x2c},
    {0xe0, 0x90, 0xdd, 0x3d, 0x06, 0x96}, {0xe2, 0x93, 0xd4, 0x36, 0x0b, 0x98},
    {0xe4, 0x96, 0xcf, 0x2b, 0x1c, 0x8a}, {0xe6, 0x95, 0xc6, 0x20, 0x11, 0x84},
    {0xe8, 0x9c, 0xf9, 0x11, 0x32, 0xae}, {0xea, 0x9f, 0xf0, 0x1a, 0x3f, 0xa0},
    {0xec, 0x9a, 0xeb, 0x07, 0x28, 0xb2}, {0xee, 0x99, 0xe2, 0x0c, 0x25, 0xbc},
    {0xf0, 0x88, 0x95, 0x65, 0x6e, 0xe6}, {0xf2, 0x8b, 0x9c, 0x6e, 0x63, 0xe8},
    {0xf4, 0x8e, 0x87, 0x73, 0x74, 0xfa}, {0xf6, 0x8d, 0x8e, 0x78, 0x79, 0xf4},
    {0xf8, 0x84, 0xb1, 0x49, 0x5a, 0xde}, {0xfa, 0x87, 0xb8, 0x42, 0x57, 0xd0},
    {0xfc, 0x82, 0xa3, 0x5f, 0x40, 0xc2}, {0xfe, 0x81, 0xaa, 0x54, 0x4d, 0xcc},
    {0x1b, 0x9b, 0xec, 0xf7, 0xda, 0x41}, {0x19, 0x98, 0xe5, 0xfc, 0xd7, 0x4f},
    {0x1f, 0x9d, 0xfe, 0xe1, 0xc0, 0x5d}, {0x1d, 0x9e, 0xf7, 0xea, 0xcd, 0x53},
    {0x13, 0x97, 0xc8, 0xdb, 0xee, 0x79}, {0x11, 0x94, 0xc1, 0xd0, 0xe3, 0x77},
    {0x17, 0x91, 0xda, 0xcd, 0xf4, 0x65}, {0x15, 0x92, 0xd3, 0xc6, 0xf9, 0x6b},
    {0x0b, 0x83, 0xa4, 0xaf, 0xb2, 0x31}, {0x09, 0x80, 0xad, 0xa4, 0xbf, 0x3f},
    {0x0f, 0x85, 0xb6, 0xb9, 0xa8, 0x2d}, {0x0d, 0x86, 0xbf, 0xb2, 0xa5, 0x23},
    {0x03, 0x8f, 0x80, 0x83, 0x86, 0x09}, {0x01, 0x8c, 0x89, 0x88, 0x8b, 0x07},
    {0x07, 0x89, 0x92, 0x95, 0x9c, 0x15}, {0x05, 0x8a, 0x9b, 0x9e, 0x91, 0x1b},
    {0x3b, 0xab, 0x7c, 0x47, 0x0a, 0xa1}, {0x39, 0xa8, 0x75, 0x4c, 0x07, 0xaf},
    {0x3f, 0xad, 0x6e, 0x51, 0x10, 0xbd}, {0x3d, 0xae, 0x67, 0x5a, 0x1d, 0xb3},
    {0x33, 0xa7, 0x58, 0x6b, 0x3e, 0x99}, {0x31, 0xa4, 0x51, 0x60, 0x33, 0x97},
    {0x37, 0xa1, 0x4a, 0x7d, 0x24, 0x85}, {0x35, 0xa2, 0x43, 0x76, 0x29, 0x8b},
    {0x2b, 0xb3, 0x34, 0x1f, 0x62, 0xd1}, {0x29, 0xb0, 0x3d, 0x14, 0x6f, 0xdf},
    {0x2f, 0xb5, 0x26, 0x09, 0x78, 0xcd}, {0x2d, 0xb6, 0x2f, 0x02, 0x75, 0xc3},
    {0x23, 0xbf, 0x10, 0x33, 0x56, 0xe9}, {0x21, 0xbc, 0x19, 0x38, 0x5b, 0xe7},
    {0x27, 0xb9, 0x02, 0x25, 0x4c, 0xf5}, {0x25, 0xba, 0x0b, 0x2e, 0x41, 0xfb},
    {0x5b, 0xfb, 0xd7, 0x8c, 0x61, 0x9a}, {0x59, 0xf8, 0xde, 0x87, 0x6c, 0x94},
    {0x5f, 0xfd, 0xc5, 0x9a, 0x7b, 0x86}, {0x5d, 0xfe, 0xcc, 0x91, 0x76, 0x88},
    {0x53, 0xf7, 0xf3, 0xa0, 0x55, 0xa2}, {0x51, 0xf4, 0xfa, 0xab, 0x58, 0xac},
    {0x57, 0xf1, 0xe1, 0xb6, 0x4f, 0xbe}, {0x55, 0xf2, 0xe8, 0xbd, 0x42, 0xb0},
    {0x4b, 0xe3, 0x9f, 0xd4, 0x09, 0xea}, {0x49, 0xe0, 0x96, 0xdf, 0x04, 0xe4},
    {0x4f, 0xe5, 0x8d, 0xc2, 0x13, 0xf6}, {0x4d, 0xe6, 0x84, 0xc9, 0x1e, 0xf8},
    {0x43, 0xef, 0xbb, 0xf8, 0x3d, 0xd2}, {0x41, 0xec, 0xb2, 0xf3, 0x30, 0xdc},
    {0x47, 0xe9, 0xa9, 0xee, 0x27, 0xce}, {0x45, 0xea, 0xa0, 0xe5, 0x2a, 0xc0},
    {0x7b, 0xcb, 0x47, 0x3c, 0xb1, 0x7a}, {0x79, 0xc8, 0x4e, 0x37, 0xbc, 0x74},
    {0x7f, 0xcd, 0x55, 0x2a, 0xab, 0x66}, {0x7d, 0xce, 0x5c, 0x21, 0xa6, 0x68},
    {0x73, 0xc7, 0x63, 0x10, 0x85, 0x42}, {0x71, 0xc4, 0x6a, 0x1b, 0x88, 0x4c},
    {0x77, 0xc1, 0x71, 0x06, 0x9f, 0x5e}, {0x75, 0xc2, 0x78, 0x0d, 0x92, 0x50},
    {0x6b, 0xd3, 0x0f, 0x64, 0xd9, 0x0a}, {0x69, 0xd0, 0x06, 0x6f, 0xd4, 0x04},
    {0x6f, 0xd5, 0x1d, 0x72, 0xc3, 0x16}, {0x6d, 0xd6, 0x14, 0x79, 0xce, 0x18},
    {0x63, 0xdf, 0x2b, 0x48, 0xed, 0x32}, {0x61, 0xdc, 0x22, 0x43, 0xe0, 0x3c},
    {0x67, 0xd9, 0x39, 0x5e, 0xf7, 0x2e}, {0x65, 0xda, 0x30, 0x55, 0xfa, 0x20},
    {0x9b, 0x5b, 0x9a, 0x01, 0xb7, 0xec}, {0x99, 0x58, 0x93, 0x0a, 0xba, 0xe2},
    {0x9f, 0x5d, 0x88, 0x17, 0xad, 0xf0}, {0x9d, 0x5e, 0x81, 0x1c, 0xa0, 0xfe},
    {0x93, 0x57, 0xbe, 0x2d, 0x83, 0xd4}, {0x91, 0x54, 0xb7, 0x26, 0x8e, 0xda},
    {0x97, 0x51, 0xac, 0x3b, 0x99, 0xc8}, {0x95, 0x52, 0xa5, 0x30, 0x94, 0xc6},
    {0x8b, 0x43, 0xd2, 0x59, 0xdf, 0x9c}, {0x89, 0x40, 0xdb, 0x52, 0xd2, 0x92},
    {0x8f, 0x45, 0xc0, 0x4f, 0xc5, 0x80}, {0x8d, 0x46, 0xc9, 0x44, 0xc8, 0x8e},
    {0x83, 0x4f, 0xf6, 0x75, 0xeb, 0xa4}, {0x81, 0x4c, 0xff, 0x7e, 0xe6, 0xaa},
    {0x87, 0x49, 0xe4, 0x63, 0xf1, 0xb8}, {0x85, 0x4a, 0xed, 0x68, 0xfc, 0xb6},
    {0xbb, 0x6b, 0x0a, 0xb1, 0x67, 0x0c}, {0xb9, 0x68, 0x03, 0xba, 0x6a, 0x02},
    {0xbf, 0x6d, 0x18, 0xa7, 0x7d, 0x10}, {0xbd, 0x6e, 0x11, 0xac, 0x70, 0x1e},
    {0xb3, 0x67, 0x2e, 0x9d, 0x53, 0x34}, {0xb1, 0x64, 0x27, 0x96, 0x5e, 0x3a},
    {0xb7, 0x61, 0x3c, 0x8b, 0x49, 0x28}, {0xb5, 0x62, 0x35, 0x80, 0x44, 0x26},
    {0xab, 0x73, 0x42, 0xe9, 0x0f, 0x7c}, {0xa9, 0x70, 0x4b, 0xe2, 0x02, 0x72},
    {0xaf, 0x75, 0x50, 0xff, 0x15, 0x60}, {0xad, 0x76, 0x59, 0xf4, 0x18, 0x6e},
    {0xa3, 0x7f, 0x66, 0xc5, 0x3b, 0x44}, {0xa1, 0x7c, 0x6f, 0xce, 0x36, 0x4a},
    {0xa7, 0x79, 0x74, 0xd3, 0x21, 0x58}, {0xa5, 0x7a, 0x7d, 0xd8, 0x2c, 0x56},
    {0xdb, 0x3b, 0xa1, 0x7a, 0x0c, 0x37}, {0xd9, 0x38, 0xa8, 0x71, 0x01, 0x39},
    {0xdf, 0x3d, 0xb3, 0x6c, 0x16, 0x2b}, {0xdd, 0x3e, 0xba, 0x67, 0x1b, 0x25},
    {0xd3, 0x37, 0x85, 0x56, 0x38, 0x0f}, {0xd1, 0x34, 0x8c, 0x5d, 0x35, 0x01},
    {0xd7, 0x31, 0x97, 0x40, 0x22, 0x13}, {0xd5, 0x32, 0x9e, 0x4b, 0x2f, 0x1d},
    {0xcb, 0x23, 0xe9, 0x22, 0x64, 0x47}, {0xc9, 0x20, 0xe0, 0x29, 0x69, 0x49},
    {0xcf, 0x25, 0xfb, 0x34, 0x7e, 0x5b}, {0xcd, 0x26, 0xf2, 0x3f, 0x73, 0x55},
    {0xc3, 0x2f, 0xcd, 0x0e, 0x50, 0x7f}, {0xc1, 0x2c, 0xc4, 0x05, 0x5d, 0x71},
    {0xc7, 0x29, 0xdf, 0x18, 0x4a, 0x63}, {0xc5, 0x2a, 0xd6, 0x13, 0x47, 0x6d},
    {0xfb, 0x0b, 0x31, 0xca, 0xdc, 0xd7}, {0xf9, 0x08, 0x38, 0xc1, 0xd1, 0xd9},
    {0xff, 0x0d, 0x23, 0xdc, 0xc6, 0xcb}, {0xfd, 0x0e, 0x2a, 0xd7, 0xcb, 0xc5},
    {0xf3, 0x07, 0x15, 0xe6, 0xe8, 0xef}, {0xf1, 0x04, 0x1c, 0xed, 0xe5, 0xe1},
    {0xf7, 0x01, 0x07, 0xf0, 0xf2, 0xf3}, {0xf5, 0x02, 0x0e, 0xfb, 0xff, 0xfd},
    {0xeb, 0x13, 0x79, 0x92, 0xb4, 0xa7}, {0xe9, 0x10, 0x70, 0x99, 0xb9, 0xa9},
    {0xef, 0x15, 0x6b, 0x84, 0xae, 0xbb}, {0xed, 0x16, 0x62, 0x8f, 0xa3, 0xb5},
    {0xe3, 0x1f, 0x5d, 0xbe, 0x80, 0x9f}, {0xe1, 0x1c, 0x54, 0xb5, 0x8d, 0x91},
    {0xe7, 0x19, 0x4f, 0xa8, 0x9a, 0x83}, {0xe5, 0x1a, 0x46, 0xa3, 0x97, 0x8d}};

unsigned char RemoteSaveWorker::aesSBox[AES_BLOCK_SIZE][AES_BLOCK_SIZE] = {
    {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67,
        0x2b, 0xfe, 0xd7, 0xab, 0x76},
    {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2,
        0xaf, 0x9c, 0xa4, 0x72, 0xc0},
    {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5,
        0xf1, 0x71, 0xd8, 0x31, 0x15},
    {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80,
        0xe2, 0xeb, 0x27, 0xb2, 0x75},
    {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6,
        0xb3, 0x29, 0xe3, 0x2f, 0x84},
    {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe,
        0x39, 0x4a, 0x4c, 0x58, 0xcf},
    {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02,
        0x7f, 0x50, 0x3c, 0x9f, 0xa8},
    {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda,
        0x21, 0x10, 0xff, 0xf3, 0xd2},
    {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e,
        0x3d, 0x64, 0x5d, 0x19, 0x73},
    {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8,
        0x14, 0xde, 0x5e, 0x0b, 0xdb},
    {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac,
        0x62, 0x91, 0x95, 0xe4, 0x79},
    {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4,
        0xea, 0x65, 0x7a, 0xae, 0x08},
    {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74,
        0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
    {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57,
        0xb9, 0x86, 0xc1, 0x1d, 0x9e},
    {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87,
        0xe9, 0xce, 0x55, 0x28, 0xdf},
    {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d,
        0x0f, 0xb0, 0x54, 0xbb, 0x16}};

#ifdef USE_AES_DECRYPT
unsigned char RemoteSaveWorker::aesInvSBox[AES_BLOCK_SIZE][AES_BLOCK_SIZE] = {
    {0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3,
        0x9e, 0x81, 0xf3, 0xd7, 0xfb},
    {0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43,
        0x44, 0xc4, 0xde, 0xe9, 0xcb},
    {0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95,
        0x0b, 0x42, 0xfa, 0xc3, 0x4e},
    {0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2,
        0x49, 0x6d, 0x8b, 0xd1, 0x25},
    {0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c,
        0xcc, 0x5d, 0x65, 0xb6, 0x92},
    {0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46,
        0x57, 0xa7, 0x8d, 0x9d, 0x84},
    {0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58,
        0x05, 0xb8, 0xb3, 0x45, 0x06},
    {0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd,
        0x03, 0x01, 0x13, 0x8a, 0x6b},
    {0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf,
        0xce, 0xf0, 0xb4, 0xe6, 0x73},
    {0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37,
        0xe8, 0x1c, 0x75, 0xdf, 0x6e},
    {0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62,
        0x0e, 0xaa, 0x18, 0xbe, 0x1b},
    {0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0,
        0xfe, 0x78, 0xcd, 0x5a, 0xf4},
    {0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10,
        0x59, 0x27, 0x80, 0xec, 0x5f},
    {0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a,
        0x9f, 0x93, 0xc9, 0x9c, 0xef},
    {0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb,
        0x3c, 0x83, 0x53, 0x99, 0x61},
    {0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14,
        0x63, 0x55, 0x21, 0x0c, 0x7d}};
#endif // USE_AES_DECRYPT

RemoteSaveWorker::RemoteSaveWorker(Document* doc, QObject* parent) :
    QObject(parent),
    document_(doc)
{
}

RemoteSaveWorker::~RemoteSaveWorker()
{
}

void
RemoteSaveWorker::process()
{
    char buffer[BUFFER_SIZE];
    unsigned char publicKey[SHA256_BLOCK_SIZE];
    unsigned char privateKey[SHA256_BLOCK_SIZE];
    unsigned char passwordHash[SHA256_BLOCK_SIZE];
    unsigned char authenticate[SHA256_BLOCK_SIZE];
    unsigned char aesKey[SHA256_BLOCK_SIZE];
    unsigned int aesKeySchedule[AES_KEY_SCHEDULE_SIZE];
    unsigned char aesUnencrypted[AES_BLOCK_SIZE];
    unsigned char aesEncrypted[AES_BLOCK_SIZE];
    int socketID = socket(AF_INET , SOCK_STREAM , 0);
    struct sockaddr_in server;
    QString qString;
    QString doc;
    bool ok;
    unsigned int port = PreferenceDialog::get().serverPort();
    int tmp;
    int size;
    int packetSize;

    if (socketID > 0 && port > 0) {
        server.sin_addr.s_addr = inet_addr(
            PreferenceDialog::get().serverAddress().toLatin1().data());
        server.sin_family = AF_INET;
        server.sin_port = htons(port);

        size = ::connect(socketID, (struct sockaddr*) &server, sizeof(server));
        if (size >= 0) {
            ok = true;

            size = recv(socketID, buffer, BUFFER_SIZE, 0);
            if (size == SHA256_BLOCK_SIZE) {
                qString = PreferenceDialog::get().serverPrivateKey();
                if (qString.size() >= 2) {
                    if (qString.at(0) == 'x') {
                        qString.remove(0, 1);
                    } else if (qString.at(0) == '0' && qString.at(1) == 'x') {
                        qString.remove(0, 2);
                    }
                    convertHexTextToKeyBlock(privateKey, qString.toLatin1());
                }

                qString = PreferenceDialog::get().serverPassword();
                memcpy(passwordHash,
                    sha256((unsigned char*) qString.toLatin1().data(),
                        qString.size()), SHA256_BLOCK_SIZE);

                memcpy(publicKey, buffer,
                    SHA256_BLOCK_SIZE * sizeof(unsigned char));
                size = 0;
                while (size < SHA256_BLOCK_SIZE) {
                    authenticate[size] = privateKey[size] ^
                        passwordHash[size] ^ publicKey[size];
                    ++size;
                }
                memcpy(authenticate, sha256(authenticate, SHA256_BLOCK_SIZE),
                    SHA256_BLOCK_SIZE);
                size = send(socketID, authenticate, SHA256_BLOCK_SIZE, 0);
            } else {
                ok = false;
            }

            if (ok == true) {
                size = recv(socketID, buffer, BUFFER_SIZE, 0);
                if (size == 0 || buffer[0] != '0') {
                    PreferenceDialog::get().setRemoteErrorStatus(
                        PreferenceDialog::Authentication);
                    ok = false;
                }
            }

            if (ok == true) {
                size = 0;
                while (size < SHA256_BLOCK_SIZE) {
                    aesKey[size] = passwordHash[size] ^ publicKey[size];
                    ++size;
                }
                memcpy(aesKey, sha256(aesKey, SHA256_BLOCK_SIZE),
                    SHA256_BLOCK_SIZE);
                aesKeySetup(aesKeySchedule, aesKey);
            }

            if (ok == true) {
                doc = QString("%1\n").arg(document_->toPlainText());
                size = 0;

                // File size.
                tmp = doc.size();
                tmp = htonl(tmp);
                memcpy(buffer, &tmp, sizeof(unsigned int));
                size += sizeof(unsigned int);

                // Action size.
                qString = PreferenceDialog::get().serverAction();
                tmp = qString.size() + 1;
                tmp = htonl(tmp);
                memcpy(buffer + size, &tmp, sizeof(unsigned int));
                size += sizeof(unsigned int);

                // CRC32 checksum.
                tmp = crc32Checksum(doc);
                tmp = htonl(tmp);
                memcpy(buffer + size, &tmp, sizeof(unsigned int));
                size += sizeof(unsigned int);

                // Remote path size.
                qString = document_->remotePath();
                tmp = qString.size() + 1;
                tmp = htonl(tmp);
                memcpy(buffer + size, &tmp, sizeof(unsigned int));
                size += sizeof(unsigned int);

                // Remote path.
                tmp = qString.size() + 1;
                if (tmp > BUFFER_SIZE - 20) {
                    tmp = BUFFER_SIZE - 20;
                }
                memset(buffer + size, '\0', tmp);
                strncpy(reinterpret_cast<char*>(buffer + size),
                    qString.toLatin1().data(), tmp);
                size += tmp;

                tmp = 0;
                while (tmp < size) {
                    memcpy(aesUnencrypted, buffer + tmp, AES_BLOCK_SIZE);
                    aesEncrypt(aesEncrypted, aesUnencrypted, aesKeySchedule);
                    memcpy(buffer + tmp, aesEncrypted, AES_BLOCK_SIZE);

                    tmp += AES_BLOCK_SIZE;
                }
                size = tmp;

                size = send(socketID, buffer, size, 0);

                size = recv(socketID, buffer, BUFFER_SIZE, 0);
                if (size == 0 || buffer[0] != '0') {
                    PreferenceDialog::get().setRemoteErrorStatus(
                        PreferenceDialog::CannotOpen);
                    ok = false;
                }
            }

            if (ok == true) {
                size = 0;
                while (size < doc.size()) {
                    memset(buffer, '\0', BUFFER_SIZE);

                    packetSize = doc.size() - size;
                    if (packetSize > BUFFER_SIZE) {
                        packetSize = BUFFER_SIZE;
                    }
                    memcpy(buffer, doc.toLatin1().data() + size,
                        packetSize);

                    tmp = 0;
                    while (tmp < packetSize) {
                        memcpy(aesUnencrypted, buffer + tmp, AES_BLOCK_SIZE);
                        aesEncrypt(aesEncrypted, aesUnencrypted, aesKeySchedule);
                        memcpy(buffer + tmp, aesEncrypted, AES_BLOCK_SIZE);

                        tmp += AES_BLOCK_SIZE;
                    }
                    send(socketID, buffer, tmp, 0);

                    size += tmp;

                    tmp = recv(socketID, buffer, BUFFER_SIZE, 0);
                }

                size = recv(socketID, buffer, BUFFER_SIZE, 0);
                if (size == 0) {
                    PreferenceDialog::get().setRemoteErrorStatus(
                        PreferenceDialog::InvalidReceiveSize);
                    ok = false;
                } else if (buffer[0] != '0') {
                    if (buffer[0] == '6') {
                        PreferenceDialog::get().setRemoteErrorStatus(
                            PreferenceDialog::BadFileSize);
                    } else if (buffer[0] == '7') {
                        PreferenceDialog::get().setRemoteErrorStatus(
                            PreferenceDialog::BadFileChecksum);
                    }
                    ok = false;
                }
            }

            if (ok == true &&
                    PreferenceDialog::get().serverAction().isEmpty() == false) {
                qString = PreferenceDialog::get().serverAction();
                size = qString.size();
                memset(buffer, '\0', size + 1);
                memcpy(buffer, qString.toLatin1().data(), size);
                buffer[size] = '\0';
                ++size;

                tmp = 0;
                while (tmp < size) {
                    memcpy(aesUnencrypted, buffer + tmp, AES_BLOCK_SIZE);
                    aesEncrypt(aesEncrypted, aesUnencrypted, aesKeySchedule);
                    memcpy(buffer + tmp, aesEncrypted, AES_BLOCK_SIZE);

                    tmp += AES_BLOCK_SIZE;
                }
                send(socketID, buffer, tmp, 0);

                size = recv(socketID, buffer, BUFFER_SIZE, 0);
                if (size == 0) {
                    PreferenceDialog::get().setRemoteErrorStatus(
                        PreferenceDialog::InvalidReceiveSize);
                    ok = false;
                } else if (buffer[0] != '0') {
                    PreferenceDialog::get().setRemoteErrorStatus(
                        PreferenceDialog::FailedAction);
                    ok = false;
                }
            }

            if (ok == true) {
                PreferenceDialog::get().setRemoteErrorStatus(
                    PreferenceDialog::Success);
            }

            ::close(socketID);
        } else {
            PreferenceDialog::get().setRemoteErrorStatus(
                PreferenceDialog::Connection);
        }
    } else {
        PreferenceDialog::get().setRemoteErrorStatus(
            PreferenceDialog::Port);
    }

    emit finished();
}

unsigned char*
RemoteSaveWorker::sha256(unsigned char* input, unsigned int inputLength) const
{
    static unsigned char result[SHA256_BLOCK_SIZE];
    unsigned int m[64];
    unsigned int k[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
        0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa,
        0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138,
        0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624,
        0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f,
        0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
    unsigned int state[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    unsigned char data[64];
    unsigned int bitLength[2] = {0, 0};
    unsigned int dataLength = 0;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    unsigned int e;
    unsigned int f;
    unsigned int g;
    unsigned int h;
    unsigned int t1;
    unsigned int t2;
    unsigned int index;
    unsigned int index2;

    index = 0;
    while (index < inputLength) {
        data[dataLength] = input[index];
        ++dataLength;
        if (dataLength >= 64) {
            index2 = 0;
            while (index2 < 16) {
                m[index2] = (data[index2 * 4] << 24) |
                    (data[index2 * 4 + 1] << 16) |
                    (data[index2 * 4 + 2] << 8) |
                    (data[index2 * 4 + 3]);
                ++index2;
            }
            while (index2 < 64) {
                m[index2] =
                    ( ((m[index2 - 2] >> 17) | (m[index2 - 2] << 15)) ^
                      ((m[index2 - 2] >> 19) | (m[index2 - 2] << 13)) ^
                       (m[index2 - 2] >> 10) ) + m[index2 - 7] +
                    ( ((m[index2 - 15] >> 7) | (m[index2 - 15] << 25)) ^
                      ((m[index2 - 15] >> 18) | (m[index2 - 15] << 14)) ^
                       (m[index2 - 15] >> 3)) + m[index2 - 16];
                ++index2;
            }
            a = state[0];
            b = state[1];
            c = state[2];
            d = state[3];
            e = state[4];
            f = state[5];
            g = state[6];
            h = state[7];
            index2 = 0;
            while (index2 < 64) {
                t1 = ( ((e >> 6) | (e << 26)) ^ ((e >> 11) | (e << 21)) ^
                        ((e >> 25) | (e << 7)) ) +
                    ((e & f) ^ (~e & g)) + k[index2] + m[index2] + h;
                t2 = ( ((a >> 2) | (a << 30)) ^ ((a >> 13) | (a << 19)) ^
                       ((a >> 22) | (a << 10)) ) +
                    ((a & b) ^ (a & c) ^ (b & c));
                h = g;
                g = f;
                f = e;
                e = d + t1;
                d = c;
                c = b;
                b = a;
                a = t1 + t2;
                ++index2;
            }
            state[0] += a;
            state[1] += b;
            state[2] += c;
            state[3] += d;
            state[4] += e;
            state[5] += f;
            state[6] += g;
            state[7] += h;

            if (bitLength[0] > 4294966783) {
                ++bitLength[1];
            }
            bitLength[0] += 512;

            dataLength = 0;
        }
        ++index;
    }

    index = dataLength;
    if (dataLength < 56) {
        data[index] = 0x80;
        ++index;
        while (index < 56) {
            data[index] = 0x00;
            ++index;
        }
    } else {
        data[index] = 0x80;
        ++index;
        while (index < 64) {
            data[index] = 0x00;
            ++index;
        }

        index2 = 0;
        while (index2 < 16) {
            m[index2] = (data[index2 * 4] << 24) |
                (data[index2 * 4 + 1] << 16) |
                (data[index2 * 4 + 2] << 8) |
                (data[index2 * 4 + 3]);
            ++index2;
        }
        while (index2 < 64) {
            m[index2] =
                ( ((m[index2 - 2] >> 17) | (m[index2 - 2] << 15)) ^
                  ((m[index2 - 2] >> 19) | (m[index2 - 2] << 13)) ^
                   (m[index2 - 2] >> 10) ) + m[index2 - 7] +
                ( ((m[index2 - 15] >> 7) | (m[index2 - 15] << 25)) ^
                  ((m[index2 - 15] >> 18) | (m[index2 - 15] << 14)) ^
                   (m[index2 - 15] >> 3)) + m[index2 - 16];
            ++index2;
        }
        a = state[0];
        b = state[1];
        c = state[2];
        d = state[3];
        e = state[4];
        f = state[5];
        g = state[6];
        h = state[7];
        index2 = 0;
        while (index2 < 64) {
            t1 = ( ((e >> 6) | (e << 26)) ^ ((e >> 11) | (e << 21)) ^
                    ((e >> 25) | (e << 7)) ) +
                ((e & f) ^ (~e & g)) + k[index2] + m[index2] + h;
            t2 = ( ((a >> 2) | (a << 30)) ^ ((a >> 13) | (a << 19)) ^
                   ((a >> 22) | (a << 10)) ) +
                ((a & b) ^ (a & c) ^ (b & c));
            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
            ++index2;
        }
        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;
        state[4] += e;
        state[5] += f;
        state[6] += g;
        state[7] += h;

        memset(data, 0, 56);
    }
    if (bitLength[0] > 4294967295 - dataLength * 8) {
        ++bitLength[1];
    }
    bitLength[0] += dataLength * 8;
    data[63] = bitLength[0];
    data[62] = bitLength[0] >> 8;
    data[61] = bitLength[0] >> 16;
    data[60] = bitLength[0] >> 24;
    data[59] = bitLength[1];
    data[58] = bitLength[1] >> 8;
    data[57] = bitLength[1] >> 16;
    data[56] = bitLength[1] >> 24;

    index2 = 0;
    while (index2 < 16) {
        m[index2] = (data[index2 * 4] << 24) |
            (data[index2 * 4 + 1] << 16) |
            (data[index2 * 4 + 2] << 8) |
            (data[index2 * 4 + 3]);
        ++index2;
    }
    while (index2 < 64) {
        m[index2] =
            ( ((m[index2 - 2] >> 17) | (m[index2 - 2] << 15)) ^
              ((m[index2 - 2] >> 19) | (m[index2 - 2] << 13)) ^
               (m[index2 - 2] >> 10) ) + m[index2 - 7] +
            ( ((m[index2 - 15] >> 7) | (m[index2 - 15] << 25)) ^
              ((m[index2 - 15] >> 18) | (m[index2 - 15] << 14)) ^
               (m[index2 - 15] >> 3)) + m[index2 - 16];
        ++index2;
    }
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    f = state[5];
    g = state[6];
    h = state[7];
    index2 = 0;
    while (index2 < 64) {
        t1 = ( ((e >> 6) | (e << 26)) ^ ((e >> 11) | (e << 21)) ^
                ((e >> 25) | (e << 7)) ) +
            ((e & f) ^ (~e & g)) + k[index2] + m[index2] + h;
        t2 = ( ((a >> 2) | (a << 30)) ^ ((a >> 13) | (a << 19)) ^
               ((a >> 22) | (a << 10)) ) +
            ((a & b) ^ (a & c) ^ (b & c));
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
        ++index2;
    }
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;

    index = 0;
    while (index < 4) {
        result[index] = state[0] >> (24 - index * 8) & 0x000000ff;
        result[index + 4] = state[1] >> (24 - index * 8) & 0x000000ff;
        result[index + 8] = state[2] >> (24 - index * 8) & 0x000000ff;
        result[index + 12] = state[3] >> (24 - index * 8) & 0x000000ff;
        result[index + 16] = state[4] >> (24 - index * 8) & 0x000000ff;
        result[index + 20] = state[5] >> (24 - index * 8) & 0x000000ff;
        result[index + 24] = state[6] >> (24 - index * 8) & 0x000000ff;
        result[index + 28] = state[7] >> (24 - index * 8) & 0x000000ff;
        ++index;
    }

    return result;
}

void
RemoteSaveWorker::convertHexTextToKeyBlock(
    unsigned char output[SHA256_BLOCK_SIZE],
    const QByteArray& text) const
{
    int index = 0;
    unsigned char top;
    unsigned char bottom;
    char curr;

    while (index < text.size() / 2 && index < SHA256_BLOCK_SIZE) {
        top = 0x00;
        bottom = 0x00;

        curr = text.at(index * 2);
        if (curr == '1') {
            top = 0x10;
        } else if (curr == '2') {
            top = 0x20;
        } else if (curr == '3') {
            top = 0x30;
        } else if (curr == '4') {
            top = 0x40;
        } else if (curr == '5') {
            top = 0x50;
        } else if (curr == '6') {
            top = 0x60;
        } else if (curr == '7') {
            top = 0x70;
        } else if (curr == '8') {
            top = 0x80;
        } else if (curr == '9') {
            top = 0x90;
        } else if (curr == 'a' || curr == 'A') {
            top = 0xa0;
        } else if (curr == 'b' || curr == 'B') {
            top = 0xb0;
        } else if (curr == 'c' || curr == 'C') {
            top = 0xc0;
        } else if (curr == 'd' || curr == 'D') {
            top = 0xd0;
        } else if (curr == 'e' || curr == 'E') {
            top = 0xe0;
        } else if (curr == 'f' || curr == 'F') {
            top = 0xf0;
        }

        if (index * 2 + 1 < text.size()) {
            curr = text.at(index * 2 + 1);
            if (curr == '1') {
                bottom = 0x01;
            } else if (curr == '2') {
                bottom = 0x02;
            } else if (curr == '3') {
                bottom = 0x03;
            } else if (curr == '4') {
                bottom = 0x04;
            } else if (curr == '5') {
                bottom = 0x05;
            } else if (curr == '6') {
                bottom = 0x06;
            } else if (curr == '7') {
                bottom = 0x07;
            } else if (curr == '8') {
                bottom = 0x08;
            } else if (curr == '9') {
                bottom = 0x09;
            } else if (curr == 'a' || curr == 'A') {
                bottom = 0x0a;
            } else if (curr == 'b' || curr == 'B') {
                bottom = 0x0b;
            } else if (curr == 'c' || curr == 'C') {
                bottom = 0x0c;
            } else if (curr == 'd' || curr == 'D') {
                bottom = 0x0d;
            } else if (curr == 'e' || curr == 'E') {
                bottom = 0x0e;
            } else if (curr == 'f' || curr == 'F') {
                bottom = 0x0f;
            }
        }

        output[index] = top | bottom;

        ++index;
    }
    while (index < SHA256_BLOCK_SIZE) {
        output[index] = 0x00;
        ++index;
    }
}

void
RemoteSaveWorker::aesAddRoundKey(
    unsigned char state[4][4],
    const unsigned int keySchedule[4])
{
    unsigned char subKey[4];

    subKey[0] = keySchedule[0] >> 24;
    subKey[1] = keySchedule[0] >> 16;
    subKey[2] = keySchedule[0] >> 8;
    subKey[3] = keySchedule[0];
    state[0][0] ^= subKey[0];
    state[1][0] ^= subKey[1];
    state[2][0] ^= subKey[2];
    state[3][0] ^= subKey[3];

    subKey[0] = keySchedule[1] >> 24;
    subKey[1] = keySchedule[1] >> 16;
    subKey[2] = keySchedule[1] >> 8;
    subKey[3] = keySchedule[1];
    state[0][1] ^= subKey[0];
    state[1][1] ^= subKey[1];
    state[2][1] ^= subKey[2];
    state[3][1] ^= subKey[3];

    subKey[0] = keySchedule[2] >> 24;
    subKey[1] = keySchedule[2] >> 16;
    subKey[2] = keySchedule[2] >> 8;
    subKey[3] = keySchedule[2];
    state[0][2] ^= subKey[0];
    state[1][2] ^= subKey[1];
    state[2][2] ^= subKey[2];
    state[3][2] ^= subKey[3];

    subKey[0] = keySchedule[3] >> 24;
    subKey[1] = keySchedule[3] >> 16;
    subKey[2] = keySchedule[3] >> 8;
    subKey[3] = keySchedule[3];
    state[0][3] ^= subKey[0];
    state[1][3] ^= subKey[1];
    state[2][3] ^= subKey[2];
    state[3][3] ^= subKey[3];
}

void
RemoteSaveWorker::aesSubByte(unsigned char state[4][4])
{
    state[0][0] = aesSBox[state[0][0] >> 4][state[0][0] & 0x0f];
    state[0][1] = aesSBox[state[0][1] >> 4][state[0][1] & 0x0f];
    state[0][2] = aesSBox[state[0][2] >> 4][state[0][2] & 0x0f];
    state[0][3] = aesSBox[state[0][3] >> 4][state[0][3] & 0x0f];
    state[1][0] = aesSBox[state[1][0] >> 4][state[1][0] & 0x0f];
    state[1][1] = aesSBox[state[1][1] >> 4][state[1][1] & 0x0f];
    state[1][2] = aesSBox[state[1][2] >> 4][state[1][2] & 0x0f];
    state[1][3] = aesSBox[state[1][3] >> 4][state[1][3] & 0x0f];
    state[2][0] = aesSBox[state[2][0] >> 4][state[2][0] & 0x0f];
    state[2][1] = aesSBox[state[2][1] >> 4][state[2][1] & 0x0f];
    state[2][2] = aesSBox[state[2][2] >> 4][state[2][2] & 0x0f];
    state[2][3] = aesSBox[state[2][3] >> 4][state[2][3] & 0x0f];
    state[3][0] = aesSBox[state[3][0] >> 4][state[3][0] & 0x0f];
    state[3][1] = aesSBox[state[3][1] >> 4][state[3][1] & 0x0f];
    state[3][2] = aesSBox[state[3][2] >> 4][state[3][2] & 0x0f];
    state[3][3] = aesSBox[state[3][3] >> 4][state[3][3] & 0x0f];
}

#ifdef USE_AES_DECRYPT
void
RemoteSaveWorker::aesInvSubByte(unsigned char state[4][4])
{
    state[0][0] = aesInvSBox[state[0][0] >> 4][state[0][0] & 0x0f];
    state[0][1] = aesInvSBox[state[0][1] >> 4][state[0][1] & 0x0f];
    state[0][2] = aesInvSBox[state[0][2] >> 4][state[0][2] & 0x0f];
    state[0][3] = aesInvSBox[state[0][3] >> 4][state[0][3] & 0x0f];
    state[1][0] = aesInvSBox[state[1][0] >> 4][state[1][0] & 0x0f];
    state[1][1] = aesInvSBox[state[1][1] >> 4][state[1][1] & 0x0f];
    state[1][2] = aesInvSBox[state[1][2] >> 4][state[1][2] & 0x0f];
    state[1][3] = aesInvSBox[state[1][3] >> 4][state[1][3] & 0x0f];
    state[2][0] = aesInvSBox[state[2][0] >> 4][state[2][0] & 0x0f];
    state[2][1] = aesInvSBox[state[2][1] >> 4][state[2][1] & 0x0f];
    state[2][2] = aesInvSBox[state[2][2] >> 4][state[2][2] & 0x0f];
    state[2][3] = aesInvSBox[state[2][3] >> 4][state[2][3] & 0x0f];
    state[3][0] = aesInvSBox[state[3][0] >> 4][state[3][0] & 0x0f];
    state[3][1] = aesInvSBox[state[3][1] >> 4][state[3][1] & 0x0f];
    state[3][2] = aesInvSBox[state[3][2] >> 4][state[3][2] & 0x0f];
    state[3][3] = aesInvSBox[state[3][3] >> 4][state[3][3] & 0x0f];
}
#endif // USE_AES_DECRYPT

void
RemoteSaveWorker::aesShiftRows(unsigned char state[4][4])
{
    unsigned char tmp;

    tmp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = tmp;

    tmp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = tmp;

    tmp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = tmp;

    tmp = state[3][0];
    state[3][0] = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = tmp;
}

#ifdef USE_AES_DECRYPT
void
RemoteSaveWorker::aesInvShiftRows(unsigned char state[4][4])
{
    unsigned char tmp;

	tmp = state[1][3];
	state[1][3] = state[1][2];
	state[1][2] = state[1][1];
	state[1][1] = state[1][0];
	state[1][0] = tmp;

	tmp = state[2][3];
	state[2][3] = state[2][1];
	state[2][1] = tmp;

	tmp = state[2][2];
	state[2][2] = state[2][0];
	state[2][0] = tmp;

	tmp = state[3][3];
	state[3][3] = state[3][0];
	state[3][0] = state[3][1];
	state[3][1] = state[3][2];
	state[3][2] = tmp;
}
#endif // USE_AES_DECRYPT

void
RemoteSaveWorker::aesMixColumns(unsigned char state[4][4])
{
    unsigned char col[4];

    col[0] = state[0][0];
    col[1] = state[1][0];
    col[2] = state[2][0];
    col[3] = state[3][0];
    state[0][0] = aesGfMul[col[0]][0];
    state[0][0] ^= aesGfMul[col[1]][1];
    state[0][0] ^= col[2];
    state[0][0] ^= col[3];
    state[1][0] = col[0];
    state[1][0] ^= aesGfMul[col[1]][0];
    state[1][0] ^= aesGfMul[col[2]][1];
    state[1][0] ^= col[3];
    state[2][0] = col[0];
    state[2][0] ^= col[1];
    state[2][0] ^= aesGfMul[col[2]][0];
    state[2][0] ^= aesGfMul[col[3]][1];
    state[3][0] = aesGfMul[col[0]][1];
    state[3][0] ^= col[1];
    state[3][0] ^= col[2];
    state[3][0] ^= aesGfMul[col[3]][0];

    col[0] = state[0][1];
    col[1] = state[1][1];
    col[2] = state[2][1];
    col[3] = state[3][1];
    state[0][1] = aesGfMul[col[0]][0];
    state[0][1] ^= aesGfMul[col[1]][1];
    state[0][1] ^= col[2];
    state[0][1] ^= col[3];
    state[1][1] = col[0];
    state[1][1] ^= aesGfMul[col[1]][0];
    state[1][1] ^= aesGfMul[col[2]][1];
    state[1][1] ^= col[3];
    state[2][1] = col[0];
    state[2][1] ^= col[1];
    state[2][1] ^= aesGfMul[col[2]][0];
    state[2][1] ^= aesGfMul[col[3]][1];
    state[3][1] = aesGfMul[col[0]][1];
    state[3][1] ^= col[1];
    state[3][1] ^= col[2];
    state[3][1] ^= aesGfMul[col[3]][0];

    col[0] = state[0][2];
    col[1] = state[1][2];
    col[2] = state[2][2];
    col[3] = state[3][2];
    state[0][2] = aesGfMul[col[0]][0];
    state[0][2] ^= aesGfMul[col[1]][1];
    state[0][2] ^= col[2];
    state[0][2] ^= col[3];
    state[1][2] = col[0];
    state[1][2] ^= aesGfMul[col[1]][0];
    state[1][2] ^= aesGfMul[col[2]][1];
    state[1][2] ^= col[3];
    state[2][2] = col[0];
    state[2][2] ^= col[1];
    state[2][2] ^= aesGfMul[col[2]][0];
    state[2][2] ^= aesGfMul[col[3]][1];
    state[3][2] = aesGfMul[col[0]][1];
    state[3][2] ^= col[1];
    state[3][2] ^= col[2];
    state[3][2] ^= aesGfMul[col[3]][0];

    col[0] = state[0][3];
    col[1] = state[1][3];
    col[2] = state[2][3];
    col[3] = state[3][3];
    state[0][3] = aesGfMul[col[0]][0];
    state[0][3] ^= aesGfMul[col[1]][1];
    state[0][3] ^= col[2];
    state[0][3] ^= col[3];
    state[1][3] = col[0];
    state[1][3] ^= aesGfMul[col[1]][0];
    state[1][3] ^= aesGfMul[col[2]][1];
    state[1][3] ^= col[3];
    state[2][3] = col[0];
    state[2][3] ^= col[1];
    state[2][3] ^= aesGfMul[col[2]][0];
    state[2][3] ^= aesGfMul[col[3]][1];
    state[3][3] = aesGfMul[col[0]][1];
    state[3][3] ^= col[1];
    state[3][3] ^= col[2];
    state[3][3] ^= aesGfMul[col[3]][0];
}

#ifdef USE_AES_DECRYPT
void
RemoteSaveWorker::aesInvMixColumns(unsigned char state[4][4])
{
    unsigned char col[4];

    col[0] = state[0][0];
    col[1] = state[1][0];
    col[2] = state[2][0];
    col[3] = state[3][0];
    state[0][0] = aesGfMul[col[0]][5];
    state[0][0] ^= aesGfMul[col[1]][3];
    state[0][0] ^= aesGfMul[col[2]][4];
    state[0][0] ^= aesGfMul[col[3]][2];
    state[1][0] = aesGfMul[col[0]][2];
    state[1][0] ^= aesGfMul[col[1]][5];
    state[1][0] ^= aesGfMul[col[2]][3];
    state[1][0] ^= aesGfMul[col[3]][4];
    state[2][0] = aesGfMul[col[0]][4];
    state[2][0] ^= aesGfMul[col[1]][2];
    state[2][0] ^= aesGfMul[col[2]][5];
    state[2][0] ^= aesGfMul[col[3]][3];
    state[3][0] = aesGfMul[col[0]][3];
    state[3][0] ^= aesGfMul[col[1]][4];
    state[3][0] ^= aesGfMul[col[2]][2];
    state[3][0] ^= aesGfMul[col[3]][5];

    col[0] = state[0][1];
    col[1] = state[1][1];
    col[2] = state[2][1];
    col[3] = state[3][1];
    state[0][1] = aesGfMul[col[0]][5];
    state[0][1] ^= aesGfMul[col[1]][3];
    state[0][1] ^= aesGfMul[col[2]][4];
    state[0][1] ^= aesGfMul[col[3]][2];
    state[1][1] = aesGfMul[col[0]][2];
    state[1][1] ^= aesGfMul[col[1]][5];
    state[1][1] ^= aesGfMul[col[2]][3];
    state[1][1] ^= aesGfMul[col[3]][4];
    state[2][1] = aesGfMul[col[0]][4];
    state[2][1] ^= aesGfMul[col[1]][2];
    state[2][1] ^= aesGfMul[col[2]][5];
    state[2][1] ^= aesGfMul[col[3]][3];
    state[3][1] = aesGfMul[col[0]][3];
    state[3][1] ^= aesGfMul[col[1]][4];
    state[3][1] ^= aesGfMul[col[2]][2];
    state[3][1] ^= aesGfMul[col[3]][5];

    col[0] = state[0][2];
    col[1] = state[1][2];
    col[2] = state[2][2];
    col[3] = state[3][2];
    state[0][2] = aesGfMul[col[0]][5];
    state[0][2] ^= aesGfMul[col[1]][3];
    state[0][2] ^= aesGfMul[col[2]][4];
    state[0][2] ^= aesGfMul[col[3]][2];
    state[1][2] = aesGfMul[col[0]][2];
    state[1][2] ^= aesGfMul[col[1]][5];
    state[1][2] ^= aesGfMul[col[2]][3];
    state[1][2] ^= aesGfMul[col[3]][4];
    state[2][2] = aesGfMul[col[0]][4];
    state[2][2] ^= aesGfMul[col[1]][2];
    state[2][2] ^= aesGfMul[col[2]][5];
    state[2][2] ^= aesGfMul[col[3]][3];
    state[3][2] = aesGfMul[col[0]][3];
    state[3][2] ^= aesGfMul[col[1]][4];
    state[3][2] ^= aesGfMul[col[2]][2];
    state[3][2] ^= aesGfMul[col[3]][5];

    col[0] = state[0][3];
    col[1] = state[1][3];
    col[2] = state[2][3];
    col[3] = state[3][3];
    state[0][3] = aesGfMul[col[0]][5];
    state[0][3] ^= aesGfMul[col[1]][3];
    state[0][3] ^= aesGfMul[col[2]][4];
    state[0][3] ^= aesGfMul[col[3]][2];
    state[1][3] = aesGfMul[col[0]][2];
    state[1][3] ^= aesGfMul[col[1]][5];
    state[1][3] ^= aesGfMul[col[2]][3];
    state[1][3] ^= aesGfMul[col[3]][4];
    state[2][3] = aesGfMul[col[0]][4];
    state[2][3] ^= aesGfMul[col[1]][2];
    state[2][3] ^= aesGfMul[col[2]][5];
    state[2][3] ^= aesGfMul[col[3]][3];
    state[3][3] = aesGfMul[col[0]][3];
    state[3][3] ^= aesGfMul[col[1]][4];
    state[3][3] ^= aesGfMul[col[2]][2];
    state[3][3] ^= aesGfMul[col[3]][5];
}
#endif // USE_AES_DECRYPT

unsigned int
RemoteSaveWorker::aesSubWord(unsigned int word)
{
    unsigned int result;

	result =  (int) aesSBox[(word >> 4 ) & 0x0000000f][word         & 0x0000000f];
	result += (int) aesSBox[(word >> 12) & 0x0000000f][(word >> 8 ) & 0x0000000f] << 8;
	result += (int) aesSBox[(word >> 20) & 0x0000000f][(word >> 16) & 0x0000000f] << 16;
	result += (int) aesSBox[(word >> 28) & 0x0000000f][(word >> 24) & 0x0000000f] << 24;

    return result;
}

void
RemoteSaveWorker::aesKeySetup(
    unsigned int output[AES_KEY_SCHEDULE_SIZE],
    const unsigned char key[AES_KEY_SIZE])
{
    static unsigned int rCon[] = {0x01000000, 0x02000000, 0x04000000,
        0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x1b000000,
        0x36000000, 0x6c000000, 0xd8000000, 0xab000000, 0x4d000000, 0x9a000000};
    unsigned int index;
    unsigned int tmp;

    index = 0;
    while (index < 8) {
        output[index] = (key[4 * index] << 24) |
            (key[4 * index + 1] << 16) | (key[4 * index + 2] << 8) |
            key[4 * index + 3];
        ++index;
    }

    index = 8;
    while (index < AES_KEY_SCHEDULE_SIZE) {
        tmp = output[index - 1];

        if ((index % 8) == 0) {
            tmp = aesSubWord((tmp << 8) | (tmp >> 24)) ^ rCon[(index - 1) / 8];
        } else if ((index % 8) == 4) {
            tmp = aesSubWord(tmp);
        }

        output[index] = output[index - 8] ^ tmp;

        ++index;
    }
}

void
RemoteSaveWorker::aesEncrypt(
    unsigned char output[AES_BLOCK_SIZE],
    const unsigned char input[AES_BLOCK_SIZE],
    const unsigned int keySchedule[AES_KEY_SCHEDULE_SIZE])
{
    unsigned char state[4][4];

    state[0][0] = input[0];
    state[1][0] = input[1];
    state[2][0] = input[2];
    state[3][0] = input[3];
    state[0][1] = input[4];
    state[1][1] = input[5];
    state[2][1] = input[6];
    state[3][1] = input[7];
    state[0][2] = input[8];
    state[1][2] = input[9];
    state[2][2] = input[10];
    state[3][2] = input[11];
    state[0][3] = input[12];
    state[1][3] = input[13];
    state[2][3] = input[14];
    state[3][3] = input[15];

    aesAddRoundKey(state, keySchedule);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 4);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 8);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 12);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 16);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 20);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 24);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 28);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 32);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 36);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 40);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 44);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 48);

    aesSubByte(state);
    aesShiftRows(state);
    aesMixColumns(state);
    aesAddRoundKey(state, keySchedule + 52);

    aesSubByte(state);
    aesShiftRows(state);
    aesAddRoundKey(state, keySchedule + 56);

    output[0] = state[0][0];
    output[1] = state[1][0];
    output[2] = state[2][0];
    output[3] = state[3][0];
    output[4] = state[0][1];
    output[5] = state[1][1];
    output[6] = state[2][1];
    output[7] = state[3][1];
    output[8] = state[0][2];
    output[9] = state[1][2];
    output[10] = state[2][2];
    output[11] = state[3][2];
    output[12] = state[0][3];
    output[13] = state[1][3];
    output[14] = state[2][3];
    output[15] = state[3][3];
}

#ifdef USE_AES_DECRYPT
void
RemoteSaveWorker::aesDecrypt(
    unsigned char output[AES_BLOCK_SIZE],
    const unsigned char input[AES_BLOCK_SIZE],
    const unsigned int keySchedule[AES_KEY_SCHEDULE_SIZE])
{
    unsigned char state[4][4];

    state[0][0] = input[0];
    state[1][0] = input[1];
    state[2][0] = input[2];
    state[3][0] = input[3];
    state[0][1] = input[4];
    state[1][1] = input[5];
    state[2][1] = input[6];
    state[3][1] = input[7];
    state[0][2] = input[8];
    state[1][2] = input[9];
    state[2][2] = input[10];
    state[3][2] = input[11];
    state[0][3] = input[12];
    state[1][3] = input[13];
    state[2][3] = input[14];
    state[3][3] = input[15];

    aesAddRoundKey(state, keySchedule + 56);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 52);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 48);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 44);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 40);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 36);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 32);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 28);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 24);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 20);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 16);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 12);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 8);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule + 4);
    aesInvMixColumns(state);

    aesInvShiftRows(state);
    aesInvSubByte(state);
    aesAddRoundKey(state, keySchedule);

    output[0] = state[0][0];
    output[1] = state[1][0];
    output[2] = state[2][0];
    output[3] = state[3][0];
    output[4] = state[0][1];
    output[5] = state[1][1];
    output[6] = state[2][1];
    output[7] = state[3][1];
    output[8] = state[0][2];
    output[9] = state[1][2];
    output[10] = state[2][2];
    output[11] = state[3][2];
    output[12] = state[0][3];
    output[13] = state[1][3];
    output[14] = state[2][3];
    output[15] = state[3][3];
}
#endif // USE_AES_DECRYPT

unsigned int
RemoteSaveWorker::crc32Checksum(const QString& input) const
{
    unsigned int result = 0xffffffff;
    unsigned int byte;
    int index = 0;
    unsigned char index2;

    while (index < input.size()) {
        byte = input.at(index).toLatin1();
        byte = ((byte & 0x55555555) <<  1) | ((byte >>  1) & 0x55555555);
        byte = ((byte & 0x33333333) <<  2) | ((byte >>  2) & 0x33333333);
        byte = ((byte & 0x0f0f0f0f) <<  4) | ((byte >>  4) & 0x0f0f0f0f);
        byte = (byte << 24) | ((byte & 0xff00) << 8) |
            ((byte >> 8) & 0xff00) | (byte >> 24);

        index2 = 0;
        while (index2 < 8) {
            if (((result ^ byte) & 0x80000000) != 0) {
                result = (result << 1) ^ 0x04c11db7;
            } else {
                result = result << 1;
            }
            byte = byte << 1;

            ++index2;
        }

        ++index;
    }

    result = ~result;
    result = ((result & 0x55555555) <<  1) | ((result >>  1) & 0x55555555);
    result = ((result & 0x33333333) <<  2) | ((result >>  2) & 0x33333333);
    result = ((result & 0x0f0f0f0f) <<  4) | ((result >>  4) & 0x0f0f0f0f);
    result = (result << 24) | ((result & 0xff00) << 8) |
        ((result >> 8) & 0xff00) | (result >> 24);

    return result;
}
