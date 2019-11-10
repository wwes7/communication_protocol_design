// server.cpp : 
//

#ifndef _CRT_SECURE_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS

#endif

#define feclong 6


#define framelong_send 3173
#define datalong_send 3168


#define framelong_receive 3173
#define datalong_receive 3168
#define length_A 3500


#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <errno.h>

#include <sys/types.h>

#include <winsock2.h>

#include <WS2tcpip.h>

#include <iostream>

#include "winsock.h"

#include "conio.h"

#include <fcntl.h>

typedef unsigned int uint;
char p[feclong] = { 1, 0, 0, 1, 1, 1 };//CRC用 
#pragma  comment(lib,"ws2_32.lib")//静态库
const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void binlengthtobin(int binlength, char imagesize[])//将binlength转为二进制
{
	unsigned long test;
	int i;
	test = 1;
	test = test << 23;
	for (i = 0; i < 24; i++) {
		if (test & binlength) {
			imagesize[i] = 1;
		}
		else {
			imagesize[i] = 0;
		}
		test = test >> 1;
	}
}
void base64_encode(char * bindata, char * base64, int binlength)
{
	int i, j;
	unsigned char current;
	for (i = 0, j = 0; i < binlength; i += 3)
	{
		current = (bindata[i] >> 2);
		current &= (unsigned char)0x3F;
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(bindata[i] << 4)) & ((unsigned char)0x30);
		if (i + 1 >= binlength)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 1] >> 4)) & ((unsigned char)0x0F);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(bindata[i + 1] << 2)) & ((unsigned char)0x3C);
		if (i + 2 >= binlength)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 2] >> 6)) & ((unsigned char)0x03);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)bindata[i + 2]) & ((unsigned char)0x3F);
		base64[j++] = base64char[(int)current];
	}
	base64[j] = '\0';

}
int base64tobit(char * base64, char * bit, int binlength)
{
	unsigned int test = 1;
	test = test << 7;
	int bitlong = 0;
	for (int i = 0; i<binlength * 2; i++) {
		for (int j = 0; j<8; j++) {
			if (test&base64[i]) {
				bit[i * 8 + j] = 1;
			}
			else {
				bit[i * 8 + j] = 0;
			}
			bitlong = i * 8 + j + 1;
			test = test >> 1;

		}
		test = 1 << 7;

	}
	return bitlong;
}
void base64_decode(char * base64, char * bindata)
{
	int i, j;
	unsigned char k;
	unsigned char temp[4];
	for (i = 0, j = 0; base64[i] != '\0'; i += 4)
	{
		memset(temp, 0xFF, sizeof(temp));
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i])
				temp[0] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 1])
				temp[1] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 2])
				temp[2] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 3])
				temp[3] = k;
		}

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2)) & 0xFC)) |
			((unsigned char)((unsigned char)(temp[1] >> 4) & 0x03));
		if (base64[i + 2] == '=')
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4)) & 0xF0)) |
			((unsigned char)((unsigned char)(temp[2] >> 2) & 0x0F));
		if (base64[i + 3] == '=')
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6)) & 0xF0)) |
			((unsigned char)(temp[3] & 0x3F));
	}
}
void bittobase64(char * base64, char * bit, int binlength)
{
	unsigned int test2 = 1 << 7;
	for (int i = 0; i<binlength * 2; i++) {
		base64[i] = '0' - '0';
		for (int j = 0; j<8; j++) {
			if (bit[i * 8 + j] == 0) {
				test2 = test2 >> 1;
			}
			else {
				base64[i] = base64[i] + (char)test2;
				test2 = test2 >> 1;
			}
		}
		test2 = 1 << 7;
	}
}

int Output_imagelong(char pro[3168]) //图片大小，输入分块数组
{
	int imagelong = 0;
	char imagesize[24];
	for (int k = 0; k<24; k++) {
		imagesize[k] = pro[k + 8];
	}
	unsigned int test = 1 << 23;
	for (int k = 0; k<23; k++) {
		if (imagesize[k] == 1) {
			imagelong = (test | 0) + imagelong;
		}
		test = test >> 1;
	}
	return imagelong;
}
int Output_bitlong(char pro[3168]) //图片大小，输入分块数组
{
	int bitlong = 0;
	char bitsize[24];
	for (int k = 0; k<24; k++) {
		bitsize[k] = pro[k + 32];
	}
	unsigned int test = 1 << 23;
	for (int k = 0; k<23; k++) {
		if (bitsize[k] == 1) {
			bitlong = (test | 0) + bitlong;
		}
		test = test >> 1;
	}
	return bitlong;
}
void Output(char *ori, int bitlong, char pro[][3168]) //数据，输入图片大小，分块数组
{
	int j = 1;
	for (int k = 0; k<bitlong;) {

		for (int i = 8; i<3168; i++) {
			ori[k] = pro[j][(k % 3160) + 8];
			k++;
		}
		j++;

	}

}

void Add(char array_A[], char array_B[], int flag)
{
	int i;
	array_B[0] = flag;
	for (i = 0; i < length_A; i++)
	{
		array_B[i + 1] = array_A[i];
	}

}
void Sub(char B[], char A[])
{
	int i = 0;
	for (i; i < length_A; i++)
	{
		A[i] = B[i + 1];
	}
}

void acktobin(int ack, char Ack[])//将ack转为二进制
{
	unsigned long test;
	int i;
	test = 1;
	test = test << 1;
	for (i = 0; i < 2; i++) {
		if (test & ack) {
			Ack[i] = 1;
		}
		else {
			Ack[i] = 0;
		}
		test = test >> 1;
	}

}
int length(char a[], int maxlength) {
	int i, m;
	for (i = 0, m = 0; i < maxlength; i++) {
		if ((a[i] != 0) && (a[i] != 1)) {
			break;
		}
		else if (a[i] == '/0') {
			break;
		}
		else {
			m++;
		}
	}
	return m;
}//计算数组已填充长度

void outkernel(char raw[], char b[]) {
	int len, m;
	len = length(raw, 1100);
	m = 0;
	for (int i = 0; i < len; i++) {
		if (i % 32 < 8) {
			m++;
		}
		else b[i - m] = raw[i];
	}
}//去除seqack
void tranlate(char rec[], char output[]) {
	int test, i;
	char yong[800];
	outkernel(rec, yong);
	int len = length(output, 150);
	for (int j = 0; j < 100; j++) {
		output[j + len] = 0;
		for (i = 0; i < 8; i++) {
			test = 1;
			test = test << (7 - i);
			test = test | output[j + len];
			if (yong[i + 8 * j] == 1) {
				output[j + len] = output[j + len] | test;
			}
			else {
				output[j + len] = output[j + len] & test;
			}
		}

	}
}//解码
void CRC(char data[], char crc_data[], char p[])
{

	char sub[feclong];
	char data_long[framelong_send];

	for (int i = 0; i < datalong_send; i++)
	{
		data_long[i] = data[i];
	}

	for (int i = datalong_send; i < framelong_send; i++)
	{
		data_long[i] = 0;
	}
	for (int i = 0; i < feclong; i++)
	{
		sub[i] = data_long[i];
	}


	for (int i = 0; i < datalong_send; i++)
	{

		for (int j = 1; j < feclong; j++)
		{
			if (sub[j] == p[j])
			{
				sub[j - 1] = 0;
			}
			else
			{
				sub[j - 1] = 1;
			}
		}
		sub[feclong - 1] = data_long[i + feclong];

		while (sub[0] == 0 && i< (datalong_send - 1))
		{

			for (int k = 0; k < (feclong - 1); k++)
			{
				sub[k] = sub[k + 1];
			}
			i++;
			sub[(feclong - 1)] = data_long[i + feclong];
		}
	}





	for (int i = 0; i < datalong_send; i++)
		crc_data[i] = data[i];

	for (int i = datalong_send; i < framelong_send; i++)
		crc_data[i] = sub[i - datalong_send];




}
int check_CRC(char data[], char crc_data[], char p[])
{

	char sub[feclong];
	for (int i = 0; i < feclong; i++)
		sub[i] = crc_data[i];
	char data_long[framelong_receive];
	for (int i = 0; i < datalong_receive; i++)
		data_long[i] = data[i];
	for (int i = datalong_receive; i < framelong_receive; i++)
		data_long[i] = 0;

	for (int i = 0; i < datalong_receive; i++)
	{

		for (int j = 1; j < feclong; j++)
		{
			if (sub[j] == p[j])
				sub[j - 1] = 0;
			else sub[j - 1] = 1;

		}
		sub[feclong - 1] = crc_data[i + feclong];

		while (sub[0] == 0 && i < (datalong_receive - 1))
		{
			if (sub[0] == 0)
			{
				for (int k = 0; k < (feclong - 1); k++)
					sub[k] = sub[k + 1];
				i++;
				sub[(feclong - 1)] = crc_data[i + feclong];
			}
		}
	}
	int check = 0;
	for (int i = 0; i < feclong - 1; i++)
		check = check + sub[i];
	if (check == 0)
	{
		for (int i = 0; i < datalong_receive; i++)
		{
			data[i] = crc_data[i];
		}
		return 0;
	}
	else
		return 1;
}
int length_Input(char a[], int maxlength) {
	int i, m;
	for (i = 0, m = 0; i < maxlength; i++) {
		if (a[i] == 0) {
			break;
		}
		else {
			m++;
		}
	}
	return m;
}//计算数组已填充长度
int check(char *frame, char restore_data[], int l)//l为frame的长度，可用sizeof 
{
	//先去帧
	//再去填充
	//再CRC
	int i = 0;
	int m = 0;
	for (i = 0; i < l - 16 - m; i++)
	{
		if ((i>4) && (frame[i + 7 + m] == 1) && (frame[i + 6 + m] == 1) && (frame[i + 5 + m] == 1) && (frame[i + 4 + m] == 1) && (frame[i + 3 + m] == 1))
		{
			m++;
		}
		restore_data[i] = frame[i + 8 + m];
		//		printf("%d", restore_data[i]);
	}
	//去CRC
	if (check_CRC(restore_data, restore_data, p) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	//正确返回1,正确数据就是restore_data

	//反之返回0
}
int event(char newdata[], char restore_data[], int receival, int length)
{
	int back = -1;
	if (receival == 1)
	{
		if (check(newdata, restore_data, length))
		{
			back = 0;
		}
		else {
			back = 1;
		}
	}
	else { back = 1; }
	return back;
}

void make_frame(char crc_data[], char frame[]) {
	int l, m, n, i;
	l = framelong_send;
	char a[8] = { 0, 1, 1, 1, 1, 1, 1, 0 };
	for (i = 0; i < 8; i++) {
		frame[i] = a[i];
	}
	for (i = 0, m = 0; i < l; i++) {
		if ((i>4) && (frame[i + 3 + m] == 1) && (frame[i + 4 + m] == 1) && (frame[i + 5 + m] == 1) && (frame[i + 6 + m] == 1) && (frame[i + 7 + m] == 1)) {
			frame[i + 8 + m] = 0;
			m++;
		}
		frame[i + 8 + m] = crc_data[i];
	}
	for (i = 0; i < 8; i++) {
		frame[i + 8 + m + l] = a[i];
	}
}
void prepare_ack(char frame[], int *ack)//发送
{
	char Ack[2];
	char crc_ack[framelong_send] = { 0 };
	acktobin(*ack, Ack);
	CRC(Ack, crc_ack, p);
	make_frame(crc_ack, frame);
}
void uncode_goal(char restore_data[], int* seq)
{
	if (restore_data[1] == 0 && restore_data[2] == 0) {
		*seq = 0;
	}
	else if (restore_data[1] == 0 && restore_data[2] == 1) {
		*seq = 1;
	}
	else if (restore_data[1] == 1 && restore_data[2] == 0) {
		*seq = 2;
	}
	else if (restore_data[1] == 1 && restore_data[2] == 1) {
		*seq = 3;
	}
}
void uncode(char restore_data[], int* seq)
{

	/*	if (restore_data[0] == 0 && restore_data[1] == 0) {
	*seq = 0;
	}
	else if (restore_data[0] == 0 && restore_data[1] == 1) {
	*seq = 1;
	}
	else if (restore_data[0] == 1 && restore_data[1] == 0) {
	*seq = 2;
	}
	else if (restore_data[0] == 1 && restore_data[1] == 1) {
	*seq = 3;
	}
	*/
	/*	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 0) {
	*seq = 8;
	}*/
	/*
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 0) {
	*seq = 0;
	}
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 1) {
	*seq = 1;
	}
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 0) {
	*seq = 2;
	}
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 1) {
	*seq = 3;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 0) {
	*seq = 4;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 1) {
	*seq = 5;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 0) {
	*seq = 6;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 1) {
	*seq = 7;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 0) {
	*seq = 8;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 1) {
	*seq = 9;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 0) {
	*seq = 10;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 1) {
	*seq = 11;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 0) {
	*seq = 12;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 1) {
	*seq = 13;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 0) {
	*seq = 14;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 1) {
	*seq = 15;
	}
	*/
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 0 && restore_data[4] == 0) {
		*seq = 0;
	}
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 0 && restore_data[4] == 1) {
		*seq = 1;
	}
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 1 && restore_data[4] == 0) {
		*seq = 2;
	}
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 1 && restore_data[4] == 1) {
		*seq = 3;
	}
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 0 && restore_data[4] == 0) {
		*seq = 4;
	}
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 0 && restore_data[4] == 1) {
		*seq = 5;
	}
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 1 && restore_data[4] == 0) {
		*seq = 6;
	}
	if (restore_data[0] == 0 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 1 && restore_data[4] == 1) {
		*seq = 7;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 0 && restore_data[4] == 0) {
		*seq = 8;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 0 && restore_data[4] == 1) {
		*seq = 9;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 1 && restore_data[4] == 0) {
		*seq = 10;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 1 && restore_data[4] == 1) {
		*seq = 11;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 0 && restore_data[4] == 0) {
		*seq = 12;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 0 && restore_data[4] == 1) {
		*seq = 13;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 1 && restore_data[4] == 0) {
		*seq = 14;
	}
	if (restore_data[0] == 0 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 1 && restore_data[4] == 1) {
		*seq = 15;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 0 && restore_data[4] == 0) {
		*seq = 16;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 0 && restore_data[4] == 1) {
		*seq = 17;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 1 && restore_data[4] == 0) {
		*seq = 18;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 0 && restore_data[3] == 1 && restore_data[4] == 1) {
		*seq = 19;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 0 && restore_data[4] == 0) {
		*seq = 20;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 0 && restore_data[4] == 1) {
		*seq = 21;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 1 && restore_data[4] == 0) {
		*seq = 22;
	}
	if (restore_data[0] == 1 && restore_data[1] == 0 && restore_data[2] == 1 && restore_data[3] == 1 && restore_data[4] == 1) {
		*seq = 23;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 0 && restore_data[4] == 0) {
		*seq = 24;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 0 && restore_data[4] == 1) {
		*seq = 25;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 1 && restore_data[4] == 0) {
		*seq = 26;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 0 && restore_data[3] == 1 && restore_data[4] == 1) {
		*seq = 27;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 0 && restore_data[4] == 0) {
		*seq = 28;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 0 && restore_data[4] == 1) {
		*seq = 29;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 1 && restore_data[4] == 0) {
		*seq = 30;
	}
	if (restore_data[0] == 1 && restore_data[1] == 1 && restore_data[2] == 1 && restore_data[3] == 1 && restore_data[4] == 1) {
		*seq = 31;
	}
}

int find(char rawdata[], char newdata[], int length, int* frame_length)
{
	int i, m, n, p, receival;
	p = 0;
	for (i = 0, n = 0, m = 0; i < 3592; i++) {
		if (rawdata[i] == 0 && rawdata[i + 1] == 1 && rawdata[i + 2] == 1 && rawdata[i + 3] == 1 && rawdata[i + 4] == 1 && rawdata[i + 5] == 1 && rawdata[i + 6] == 1 && rawdata[i + 7] == 0) {
			if (p == 0) {
				m = i;
				p = 1;
			}
			else if (p == 1) {
				n = i;
				p = 2;
			}
			else p = 3;
		}
	}
	if (n != 0 && p != 3)
	{
		for (i = 0; i < n - m + 8; i++) {
			newdata[i] = rawdata[i + m];
		}
		receival = 1;
		*frame_length = n - m + 8;
	}
	else receival = 0;

	return receival;//返回是否读到有效帧
}
int uncode_send(char restore_data[], int seq, int ack)
{
	//解码解出seq,ack再判断

	if (seq == ack)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/******************************************************************/
struct socket_list//socket数组 
{
	SOCKET MainSock;
	unsigned short MainPort;//应用层的端口号
	int num; //接口数量
	SOCKET sock_array[64];
	unsigned short port_array[64];//各接口模块的端口号
};
void init_list(socket_list *list)//初始化socket数组 
{
	int i;
	list->MainSock = 0;
	list->num = 0;
	for (i = 0; i < 64; i++) {
		list->sock_array[i] = 0;
	}
}
void insert_list(SOCKET s, unsigned short port, socket_list *list)//将指定套接字放入socket数组空位置 
{
	int i;
	for (i = 0; i < 64; i++) {
		if (list->sock_array[i] == 0) {
			list->sock_array[i] = s;
			list->port_array[i] = port;
			list->num += 1;
			break;
		}
	}
}
void delete_list(SOCKET s, socket_list *list)//从socket数组中删除指令套接字 
{
	int i;
	for (i = 0; i < 64; i++) {
		if (list->sock_array[i] == s) {
			list->sock_array[i] = 0;
			list->num -= 1;
			break;
		}
	}
}
void make_fdlist(socket_list *list, fd_set *fd_list)//将socket数组放入fd数组中，准备进入select 
{
	int i;
	FD_SET(list->MainSock, fd_list);
	for (i = 0; i < 64; i++) {
		if (list->sock_array[i] > 0) {
			FD_SET(list->sock_array[i], fd_list);
		}
	}
}
void code(unsigned long x, char A[], int length)
{
	unsigned long test;
	int i;
	x = x << (length - 5);
	test = 1;
	test = test << (length - 1);
	printf("this is");
	for (i = 0; i < length; i++) {
		if (test & x) {
			A[i] = 1;
		}
		else {
			A[i] = 0;
		}
		printf("%d", A[i]);
		test = test >> 1; //本算法利用了移位操作和"与"计算，逐位测出x的每一位是0还是1.
	}
	printf("\n");
}
unsigned long decode(char A[], int length)//解码函数 
{
	unsigned long x;
	int i;

	x = 0;
	for (i = 0; i < length; i++) {
		if (A[i] == 0) {
			x = x << 1;;
		}
		else {
			x = x << 1;
			x = x | 1;
		}
	}
	return x;
}
int Input(char *ori, int imagelong, int bitlong, int dest, int sour, char pro[][3168]) //数据，图片大小，目的，源，输出分块数组
{
	int i, j, m = 1;
	int l;
	l = imagelong * 16;//l为ori长度

	char source[2];
	char desti[2];
	char imagesize[24], bitsize[24];
	binlengthtobin(imagelong, imagesize);
	binlengthtobin(bitlong, bitsize);
	acktobin(sour, source);
	acktobin(dest, desti);
	pro[0][0] = 0;
	pro[0][1] = desti[0];
	pro[0][2] = desti[1];
	pro[0][3] = 0;
	pro[0][4] = 0;
	pro[0][5] = source[0];
	pro[0][6] = source[1];
	pro[0][7] = 0;

	for (int k = 0; k<24; k++) {
		pro[0][k + 8] = imagesize[k];
	}
	for (int k = 0; k<24; k++) {
		pro[0][k + 32] = bitsize[k];
	}



	for (j = 0; j < l; j++) {
		pro[m][0] = 0;
		pro[m][1] = desti[0];
		pro[m][2] = desti[1];
		pro[m][3] = 0;
		pro[m][4] = 0;
		pro[m][5] = source[0];
		pro[m][6] = source[1];
		pro[m][7] = 0;

		for (i = 8; (i < 3168)&(j<l); i++) {
			pro[m][i] = ori[j];

			j++;

		}
		j--;
		m++;
	}
	m--;
	return m;
}
#define MAX_BUFFSIZE 3600
#define SEND_L 3500
/******************************************************************/
int main(int argc, char* argv[])
{
	SOCKET s, sock;//创建两个套接字 
	struct sockaddr_in remote_addr;//一个地址 
	int len;
	char buf[MAX_BUFFSIZE] = { 0 };//接收缓存
	char sendbuf[SEND_L] = { 1, 0, 0, 1, 0 };//测试数据
	WSAData wsa;
	int retval, selretval;
	struct socket_list sock_list;//创建socket数组 
	//准备select 
	fd_set readfds, writefds, exceptfds;
	timeval timeout;

	int intf, i;
	unsigned long arg;
	int linecount = 0;
	unsigned short* port;
	int portNum;
	int ID;
	unsigned short myPort;
	int portLen;

	WSAStartup(0x101, &wsa);


	//buf = (char *)malloc(MAX_BUFFSIZE);//动态创建4000缓存区 
	//sendbuf = (char *)malloc(MAX_BUFFSIZE);//动态创建4000发送区 

	init_list(&sock_list);//初始化socket数组 
	//从键盘输入，需要连接的API端口号
	printf("本模块是网络层,\n");
	printf("请输入本模块的设备编号：");//主要用来区分不同的设备
	scanf_s("%d", &ID);

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == SOCKET_ERROR) {
		return 0;
	}
	//设置应用层端口号，以向应用层发送数据
	printf("请输入应用层模块的端口号：");
	scanf_s("%d", &sock_list.MainPort);

	//绑定本模块端口号，以接收应用层数据
	printf("请设置本模块的端口号：");
	scanf_s("%d", &myPort);
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(myPort);
	remote_addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
	while (bind(s, (sockaddr*)&remote_addr, sizeof(remote_addr)) != 0) {
		printf("本模块的端口号已被占用，请换一个再试:");
		scanf_s("%d", &myPort);
		remote_addr.sin_port = htons(myPort);
	}
	sock_list.MainSock = s;

	//设置物理层接口端口号，以向接口发送数据
	printf("请输入接口模块的数量：");
	scanf_s("%d", &portNum);
	port = (unsigned short *)malloc(portNum * sizeof(unsigned short));
	for (i = 0; i < portNum; i++) {
		printf("请输入接口%d的api端口号(从物理层接口模块软件上获取): ", i);
		scanf_s("%d", &(port[i]));
		//可以生成多个套接字，每个套接字对应一个接口模块
		s = socket(AF_INET, SOCK_DGRAM, 0);
		if (s == SOCKET_ERROR) {
			return 0;
		}

		remote_addr.sin_family = AF_INET;
		remote_addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
		remote_addr.sin_port = htons(port[i]);
		//向api接口发送一个测试命令
		sendto(s, "connect", 8, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
		len = sizeof(remote_addr);
		retval = recvfrom(s, buf, MAX_BUFFSIZE, 0, (sockaddr*)&remote_addr, &len);
		if (retval == SOCKET_ERROR) {
			printf("接口API关联失败\n");
			return 0;
		}
		//有接收，就可以认为关联成功
		printf("接口API连接成功！\n");
		//设置套接字的状态为阻塞态，也可以不设置
		arg = 1;
		ioctlsocket(s, FIONBIO, &arg);
		//把s放入套接字队列中，今后该套接字在sock_list里的下标可以用于表示接口的编号
		insert_list(s, port[i], &sock_list);
	}

	timeout.tv_sec = 2; //设置发送数据的时间间隔为10微秒
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	/*	for(i = 0 ; i < 4000; i ++){
	sendbuf[i] = 1; //初始化数据全为1,以备测试
	}
	*/
	/**************************/
	/**********************************/
	char receive_endline[MAX_BUFFSIZE] = { 0 };
	char restore_data[SEND_L] = { 0 };
	int back = -1;
	char newdata[SEND_L] = { 0 };
	int receival = 0;
	int length1 = 0;
	int ACK = 0;
	/**********************************/
	char Sendbuf[MAX_BUFFSIZE] = { 0 };
	int _length = SEND_L;
	char rawdata[MAX_BUFFSIZE] = { 0 };
	int Ack = 0;
	char send_ack[64] = { 0 };
	int k = 1;
	int K = 1;
	int count = -1;
	int endline = 0;
	int flag;
	int host = 0;
	int intf0 = 0;
	int intf1 = 0;
	int Eng = 0;
	char blank_buf[MAX_BUFFSIZE] = { 0 };
	int rec_Imag_length = 0;
	int rec_bit_length = 0;
	/**************************/
	/*****************************/
	int Host;
	printf("\n请输入本地主机号:");
	scanf_s("%d", &Host);
	printf("\n");
	int route_choice = 0;
	int route_recv = 0;
	//int flag = 0;
	/*****************************/
	while (1)
	{
		make_fdlist(&sock_list, &readfds);
		//本例程采用了基于select机制，不断发送测试数据，和接收测试数据，也可以采用多线程，一线专发送，一线专接收的方案
		selretval = select(0, &readfds, &writefds, &exceptfds, &timeout);
		if (selretval == SOCKET_ERROR)
		{
			retval = WSAGetLastError();
			break;
		}
		else if (selretval == 0)
		{
			//定时器触发
			continue;
		}
		if (FD_ISSET(sock_list.MainSock, &readfds))
		{
			//从应用层收到数据
			portLen = sizeof(remote_addr);
			len = sizeof(remote_addr);
			retval = recvfrom(sock_list.MainSock, buf, MAX_BUFFSIZE, 0, (sockaddr*)&remote_addr, &len);

			//			retval = recvfrom(sock_list.MainSock,buf,MAX_BUFFSIZE,0,(sockaddr*)&remote_addr,&portLen);
			if (retval > 0) {
				//将数据编码为"比特流"形式
				/*for (i = 0; i < retval; i++){
				//每次编码8位
				//code(buf[i], sendbuf + i * 8, 8);
				}*/
				//判断应用层发来数据的第0位
				if (route_choice == 0)
				{
					remote_addr.sin_port = htons(sock_list.port_array[0]);
					retval = sendto(sock_list.sock_array[0], buf, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
					while (k)
					{
						FD_ZERO(&readfds);
						FD_ZERO(&writefds);
						FD_ZERO(&exceptfds);
						remote_addr.sin_port = htons(sock_list.port_array[0]);
						make_fdlist(&sock_list, &readfds);
						selretval = select(0, &readfds, &writefds, &exceptfds, &timeout);
						if (selretval == SOCKET_ERROR)
						{
							retval = WSAGetLastError();
							break;
						}
						else if (selretval == 0)
						{
							//定时器触发
							sendto(sock_list.sock_array[0], buf, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
						}
						if (FD_ISSET(sock_list.sock_array[0], &readfds))
						{
							retval = recv(sock_list.sock_array[0], rawdata, MAX_BUFFSIZE, 0);
							if (retval > 0)
							{

								printf("初始路由表已经发送到0号接口\n");
								k = 0;
							}
							else if (retval == 0)
							{
								system("pause");
							}
						}
					}
					k = 1;
					remote_addr.sin_port = htons(sock_list.port_array[1]);
					retval = sendto(sock_list.sock_array[1], buf, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
					while (k)
					{
						FD_ZERO(&readfds);
						FD_ZERO(&writefds);
						FD_ZERO(&exceptfds);
						remote_addr.sin_port = htons(sock_list.port_array[1]);
						make_fdlist(&sock_list, &readfds);
						selretval = select(0, &readfds, &writefds, &exceptfds, &timeout);
						if (selretval == SOCKET_ERROR)
						{
							retval = WSAGetLastError();
							break;
						}
						else if (selretval == 0)
						{
							//定时器触发
							sendto(sock_list.sock_array[1], buf, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
						}
						if (FD_ISSET(sock_list.sock_array[1], &readfds))
						{
							retval = recv(sock_list.sock_array[1], rawdata, MAX_BUFFSIZE, 0);
							if (retval > 0)
							{

								printf("初始路由表已经发送到1号接口\n");
								k = 0;
							}
							else if (retval == 0)
							{
								system("pause");
							}
						}
					}
					route_choice = 1;
					k = 1;

				}
				else
				{
					if (buf[0] == 0)
					{
						Sub(buf, Sendbuf);
						remote_addr.sin_port = htons(sock_list.port_array[0]);
						retval = sendto(sock_list.sock_array[0], Sendbuf, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
						while (k)
						{
							if (count == -1)
							{
								memcpy(receive_endline, Sendbuf, MAX_BUFFSIZE);
								receival = find(receive_endline, newdata, SEND_L, &length1);
								back = event(newdata, restore_data, receival, length1);
								if (back == 0)
								{
									//uncode(restore_data, &endline);
									rec_Imag_length = Output_imagelong(restore_data);
									rec_bit_length = Output_bitlong(restore_data);
									endline = rec_bit_length / 3160 + 1;
									printf("send endline %d", endline);
									printf("\n");
									count++;
								}
							}
							FD_ZERO(&readfds);
							FD_ZERO(&writefds);
							FD_ZERO(&exceptfds);
							remote_addr.sin_port = htons(sock_list.port_array[0]);
							make_fdlist(&sock_list, &readfds);
							selretval = select(0, &readfds, &writefds, &exceptfds, &timeout);
							if (selretval == SOCKET_ERROR)
							{
								retval = WSAGetLastError();
								break;
							}
							else if (selretval == 0)
							{
								//定时器触发
								sendto(sock_list.sock_array[0], Sendbuf, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
							}
							if (FD_ISSET(sock_list.MainSock, &readfds))
							{
								remote_addr.sin_port = htons(sock_list.port_array[0]);

								len = sizeof(remote_addr);
								retval = recvfrom(sock_list.MainSock, buf, MAX_BUFFSIZE, 0, (sockaddr*)&remote_addr, &len);
								Sub(buf, Sendbuf);
								remote_addr.sin_port = htons(sock_list.port_array[0]);
								retval = sendto(sock_list.sock_array[0], Sendbuf, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));

								endline--;


							}
							if (FD_ISSET(sock_list.sock_array[0], &readfds))
							{
								retval = recv(sock_list.sock_array[0], rawdata, MAX_BUFFSIZE, 0);
								if (retval > 0)
								{

									printf("将应用层数据已经发送到0号接口\n");
									printf("还剩下endline:%d", endline);
									//将Ack编码到send_ack
									acktobin(Ack, send_ack);
									remote_addr.sin_port = htons(sock_list.MainPort);
									sendto(sock_list.MainSock, send_ack, 64, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));

									if (endline == 0)
									{
										k = 0;
										count = -1;

									}

								}
								else if (retval == 0)
								{
									system("pause");
								}

							}

							/*
							char sendbuf1[100] = { 1, 0, 0, 1, 0 };//测试数据
							remote_addr.sin_port = htons(sock_list.port_array[0]);
							sendto(sock_list.sock_array[0], sendbuf1, 5, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));

							printf("将应用层数据发送到0号接口\n");
							for (i == 0; i < 5; i++)
							{
							printf("%d", buf[i]);
							}

							*/

						}
					}
					else
					{
						Sub(buf, Sendbuf);
						remote_addr.sin_port = htons(sock_list.port_array[1]);
						retval = sendto(sock_list.sock_array[1], Sendbuf, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
						while (k)
						{
							if (count == -1)
							{
								memcpy(receive_endline, Sendbuf, MAX_BUFFSIZE);
								receival = find(receive_endline, newdata, SEND_L, &length1);
								back = event(newdata, restore_data, receival, length1);
								if (back == 0)
								{
									//uncode(restore_data, &endline);
									rec_Imag_length = Output_imagelong(restore_data);
									rec_bit_length = Output_bitlong(restore_data);
									endline = rec_bit_length / 3160 + 1;
									printf("send endline %d", endline);
									printf("\n");
									count++;
								}
							}
							FD_ZERO(&readfds);
							FD_ZERO(&writefds);
							FD_ZERO(&exceptfds);
							remote_addr.sin_port = htons(sock_list.port_array[1]);
							make_fdlist(&sock_list, &readfds);
							selretval = select(0, &readfds, &writefds, &exceptfds, &timeout);
							if (selretval == SOCKET_ERROR)
							{
								retval = WSAGetLastError();
								break;
							}
							else if (selretval == 0)
							{
								//定时器触发
								sendto(sock_list.sock_array[1], Sendbuf, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
							}
							if (FD_ISSET(sock_list.MainSock, &readfds))
							{
								remote_addr.sin_port = htons(sock_list.port_array[1]);

								len = sizeof(remote_addr);
								retval = recvfrom(sock_list.MainSock, buf, MAX_BUFFSIZE, 0, (sockaddr*)&remote_addr, &len);
								Sub(buf, Sendbuf);
								remote_addr.sin_port = htons(sock_list.port_array[1]);
								retval = sendto(sock_list.sock_array[1], Sendbuf, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));

								endline--;


							}
							if (FD_ISSET(sock_list.sock_array[1], &readfds))
							{
								retval = recv(sock_list.sock_array[1], rawdata, MAX_BUFFSIZE, 0);
								if (retval > 0)
								{

									printf("将应用层数据已经发送到0号接口\n");
									printf("还剩下endline:%d", endline);
									//将Ack编码到send_ack
									acktobin(Ack, send_ack);
									remote_addr.sin_port = htons(sock_list.MainPort);
									sendto(sock_list.MainSock, send_ack, 64, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));

									if (endline == 0)
									{
										k = 0;
										count = -1;

									}

								}
								else if (retval == 0)
								{
									system("pause");
								}

							}

							/*
							char sendbuf1[100] = { 1, 0, 0, 1, 0 };//测试数据
							remote_addr.sin_port = htons(sock_list.port_array[0]);
							sendto(sock_list.sock_array[0], sendbuf1, 5, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));

							printf("将应用层数据发送到0号接口\n");
							for (i == 0; i < 5; i++)
							{
							printf("%d", buf[i]);
							}

							*/

						}
					}
				}


			}
		}
		/*

		*/
		for (intf = 0; intf < 64; intf++)
		{
			if (sock_list.sock_array[intf] == 0)
				continue;
			sock = sock_list.sock_array[intf];

			if (FD_ISSET(sock, &readfds)) {

				retval = recv(sock, buf, MAX_BUFFSIZE, 0);
				if (retval == 0) {
					closesocket(sock);
					printf("失去接口%d的关联\n", i);
					delete_list(sock, &sock_list);
					continue;
				}
				else if (retval == -1) {
					retval = WSAGetLastError();
					if (retval == WSAEWOULDBLOCK)
						continue;
					closesocket(sock);
					printf("失去接口%d的关联\n", i);
					delete_list(sock, &sock_list);
					continue;
				}
				//收到数据后，打印
				printf("%d号接口收到比特流:", intf);
				printf("\n");
				//处理程序
				//查帧，去CRC
				memcpy(receive_endline, buf, MAX_BUFFSIZE);
				receival = find(receive_endline, newdata, SEND_L, &length1);
				back = event(newdata, restore_data, receival, length1);
				if (back == 0)
				{
					//给flag赋值为目的地址

					if (route_recv<2)
					{
						route_recv++;
						remote_addr.sin_port = htons(sock_list.MainPort);
						sendto(sock_list.MainSock, restore_data, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
						printf("从%d号口收到的初始路由表上交\n", intf);
						//将Ack编码到send_ack,Ack=0;
						prepare_ack(send_ack, &ACK);
						remote_addr.sin_port = htons(sock_list.port_array[intf]);
						printf("回应%d号接口\n", intf);

						retval = sendto(sock_list.sock_array[intf], send_ack, 2, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
					}
					else
					{
						uncode_goal(restore_data, &flag);
						if (flag == Host)//本机
						{
							remote_addr.sin_port = htons(sock_list.MainPort);
							sendto(sock_list.MainSock, restore_data, _length, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
							printf("从%d号口收到的数据上交\n", intf);
							//将Ack编码到send_ack,Ack=0;
							prepare_ack(send_ack, &ACK);
							remote_addr.sin_port = htons(sock_list.port_array[intf]);
							printf("回应%d号接口\n", intf);

							retval = sendto(sock_list.sock_array[intf], send_ack, 2, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
						}
						else//转发
						{
							intf0 = intf;
							intf1 = (intf + 1) % 2;
							remote_addr.sin_port = htons(sock_list.port_array[intf1]);
							printf("转发接口");
							memcpy(sendbuf, newdata, SEND_L);
							retval = sendto(sock_list.sock_array[intf1], sendbuf, SEND_L, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
							//定义一个控制位K=1
							/*
							FD_ZERO(&readfds);
							FD_ZERO(&writefds);
							FD_ZERO(&exceptfds);
							FD_SET(sock_list.sock_array[intf0], &readfds);
							FD_SET(sock_list.sock_array[intf1], &readfds);
							selretval = select(0, &readfds, &writefds, &exceptfds, &timeout);
							if (selretval == SOCKET_ERROR)
							{
							retval = WSAGetLastError();
							break;
							}*/
							if (FD_ISSET(sock_list.sock_array[intf0], &readfds))
							{
								retval = recv(sock_list.sock_array[intf0], buf, MAX_BUFFSIZE, 0);
								if (retval > 0)
								{
									memcpy(receive_endline, buf, MAX_BUFFSIZE);
									receival = find(receive_endline, newdata, SEND_L, &length1);
									back = event(newdata, restore_data, receival, length1);
									remote_addr.sin_port = htons(sock_list.port_array[intf1]);
									printf("转发接口");
									memcpy(sendbuf, newdata, SEND_L);
									retval = sendto(sock_list.sock_array[intf1], sendbuf, SEND_L, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
								}

							}

							if (FD_ISSET(sock_list.sock_array[intf1], &readfds))
							{
								retval = recv(sock_list.sock_array[intf1], blank_buf, MAX_BUFFSIZE, 0);
								printf("收到转发回应\n");
								if (retval > 0)
								{
									remote_addr.sin_port = htons(sock_list.port_array[intf0]);
									//将Ack编码到send_ack,Ack=0;
									prepare_ack(send_ack, &ACK);
									//retval = sendto(sock_list.sock_array[intf0], send_ack, 64, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
									retval = sendto(sock_list.sock_array[intf0], send_ack, 2, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
								}
							}



						}
					}



					/*
					//为了测试，把0号接口的数据转发到1号接口，如果有1号接口的话
					if (intf == 0) {
					if (sock_list.sock_array[1] != 0) {
					remote_addr.sin_port = htons(sock_list.port_array[1]);
					printf("从0号转发到1号接口\n");
					retval = sendto(sock_list.sock_array[1], buf, 5, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
					}
					}
					//为了测试，从1号接口收到的数据，上交给应用层模块
					if (intf == 1) {
					//先把"比特流"解码成普通数据形式，每次解码8位
					for (i = 0; i < retval; i += 8){
					sendbuf[i / 8] = (char)decode(buf + i, 8);
					}

					if (back == 0)
					{
					remote_addr.sin_port = htons(sock_list.MainPort);
					sendto(sock_list.MainSock, restore_data, 64, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
					printf("从1号口收到的数据上交\n");
					prepare_ack(sendbuf, &ACK, s);
					remote_addr.sin_port = htons(sock_list.port_array[1]);
					printf("回应1号接口\n");
					retval = sendto(sock_list.sock_array[1], sendbuf, 64, 0, (sockaddr*)&remote_addr, sizeof(remote_addr));

					}
					}
					*/
				}

			}
		}
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);
	}

	closesocket(sock_list.MainSock);
	WSACleanup();
	return 0;
}