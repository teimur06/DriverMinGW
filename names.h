/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   names.h
 * Author: Asus-PC
 *
 * Created on 23 июля 2017 г., 13:24
 */

#ifndef NAMES_H
#define NAMES_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct DATA_IRP
    {
        int code;
    } DATA_IRP;

#define IOCTL_TEST \
        CTL_CODE(0x00008000,0x801,METHOD_BUFFERED,FILE_ANY_ACCESS)    

#ifdef __cplusplus
}
#endif

#endif /* NAMES_H */

