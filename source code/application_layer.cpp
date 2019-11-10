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
char p[feclong] = { 1, 0, 0, 1, 1, 1 };//CRC�� 
#pragma  comment(lib,"ws2_32.lib")//��̬��
const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void binlengthtobin(int binlength, char imagesize[])//��binlengthתΪ������
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

int Output_imagelong(char pro[3168]) //ͼƬ��С������ֿ�����
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
int Output_bitlong(char pro[3168]) //ͼƬ��С������ֿ�����
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
void Output(char *ori, int bitlong, char pro[][3168]) //���ݣ�����ͼƬ��С���ֿ�����
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

void acktobin(int ack, char Ack[])//��ackתΪ������
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
}//������������䳤��

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
}//ȥ��seqack
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
}//����
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
}//������������䳤��
int check(char *frame, char restore_data[], int l)//lΪframe�ĳ��ȣ�����sizeof 
{
	//��ȥ֡
	//��ȥ���
	//��CRC
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
	//ȥCRC
	if (check_CRC(restore_data, restore_data, p) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	//��ȷ����1,��ȷ���ݾ���restore_data

	//��֮����0
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
void prepare_ack(char frame[], int *ack)//����
{
	char Ack[2];
	char crc_ack[framelong_send] = { 0 };
	acktobin(*ack, Ack);
	CRC(Ack, crc_ack, p);
	make_frame(crc_ack, frame);
}
void uncode_goal(char restore_data[], int* seq)
{
	if (restore_data[0] == 0 && restore_data[1] == 0) {
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
	if (m != 0 && n != 0 && p != 3) {
		for (i = 0; i < n - m + 8; i++) {
			newdata[i] = rawdata[i + m];
		}
		receival = 1;
		*frame_length = n - m + 8;
	}
	else receival = 0;

	return receival;//�����Ƿ������Ч֡
}
int uncode_send(char restore_data[], int seq, int ack)
{
	//������seq,ack���ж�

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
struct socket_list//socket���� 
{
	SOCKET MainSock;
	unsigned short MainPort;//Ӧ�ò�Ķ˿ں�
	int num; //�ӿ�����
	SOCKET sock_array[64];
	unsigned short port_array[64];//���ӿ�ģ��Ķ˿ں�
};
void init_list(socket_list *list)//��ʼ��socket���� 
{
	int i;
	list->MainSock = 0;
	list->num = 0;
	for (i = 0; i < 64; i++) {
		list->sock_array[i] = 0;
	}
}
void insert_list(SOCKET s, unsigned short port, socket_list *list)//��ָ���׽��ַ���socket�����λ�� 
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
void delete_list(SOCKET s, socket_list *list)//��socket������ɾ��ָ���׽��� 
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
void make_fdlist(socket_list *list, fd_set *fd_list)//��socket�������fd�����У�׼������select 
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
		test = test >> 1; //���㷨��������λ������"��"���㣬��λ���x��ÿһλ��0����1.
	}
	printf("\n");
}
unsigned long decode(char A[], int length)//���뺯�� 
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
int Input(char *ori, int imagelong, int bitlong, int dest, int sour, char pro[][3168]) //���ݣ�ͼƬ��С��Ŀ�ģ�Դ������ֿ�����
{
	int i, j, m = 1;
	int l;
	l = imagelong * 16;//lΪori����

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
int *IP0(char rec[2][6]) {
	static int send[4];
	send[0] = 99;//0-self
	if (rec[0][2] == 0) {
		if (rec[0][1] <= rec[0][4] + rec[1][1] + rec[1][4]) {
			send[rec[0][0]] = rec[0][0];
		}
		else {
			send[rec[0][0]] = rec[1][0];
		}
	}
	else {
		if (rec[0][4] <= rec[0][1] + rec[1][1] + rec[1][4]) {
			send[rec[0][0]] = rec[0][0];
		}
		else
			send[rec[0][0]] = rec[1][0];
	}
	if (rec[1][2] == 0) {
		if (rec[1][1] <= rec[1][4] + rec[0][1] + rec[0][4]) {
			send[rec[1][0]] = rec[1][0];
		}
		else
			send[rec[1][0]] = rec[0][0];
	}
	else {
		if (rec[1][4] <= rec[1][1] + rec[0][1] + rec[0][4]) {
			send[rec[1][0]] = rec[1][0];
		}
		else
			send[rec[1][0]] = rec[0][0];
	}
	if (rec[0][1] + rec[0][4] >= rec[1][1] + rec[1][4]) {
		if (rec[0][2] != 0) {
			send[rec[0][2]] = rec[1][0];
		}
		else
			send[rec[0][5]] = rec[1][0];
	}
	else {
		if (rec[0][2] != 0) {
			send[rec[0][2]] = rec[0][0];
		}
		else {
			send[rec[0][5]] = rec[0][0];
		}
	}
	return send;
}
/*�����˳��
0 1
2 3
�����ݴ�������ʱ������Ϊ99��
���ڰ�������㣻
*/
int *IP1(char rec[2][6]) {
	static int send[4];
	send[1] = 99;//0-self
	if (rec[0][2] == 1) {
		if (rec[0][1] <= rec[0][4] + rec[1][1] + rec[1][4]) {
			send[rec[0][0]] = rec[0][0];
		}
		else {
			send[rec[0][0]] = rec[1][0];
		}
	}
	else {
		if (rec[0][4] <= rec[0][1] + rec[1][1] + rec[1][4]) {
			send[rec[0][0]] = rec[0][0];
		}
		else
			send[rec[0][0]] = rec[1][0];
	}
	if (rec[1][2] == 1) {
		if (rec[1][1] <= rec[1][4] + rec[0][1] + rec[0][4]) {
			send[rec[1][0]] = rec[1][0];
		}
		else
			send[rec[1][0]] = rec[0][0];
	}
	else {
		if (rec[1][4] <= rec[1][1] + rec[0][1] + rec[0][4]) {
			send[rec[1][0]] = rec[1][0];
		}
		else
			send[rec[1][0]] = rec[0][0];
	}
	if (rec[0][1] + rec[0][4] >= rec[1][1] + rec[1][4]) {
		if (rec[0][2] != 1) {
			send[rec[0][2]] = rec[1][0];
		}
		else
			send[rec[0][5]] = rec[1][0];
	}
	else {
		if (rec[0][2] != 1) {
			send[rec[0][2]] = rec[0][0];
		}
		else {
			send[rec[0][5]] = rec[0][0];
		}
	}
	return send;
}
/*�����˳��
0 1
2 3
�����ݴ�������ʱ������Ϊ9��
2->0����Ϊ0��
2->3����Ϊ1��
2->1��������㣻
*/
int *IP2(char rec[2][6]) {
	static int send[4];
	send[2] = 99;//0-self
	if (rec[0][2] == 2) {
		if (rec[0][1] <= rec[0][4] + rec[1][1] + rec[1][4]) {
			send[rec[0][0]] = rec[0][0];
		}
		else {
			send[rec[0][0]] = rec[1][0];
		}
	}
	else {
		if (rec[0][4] <= rec[0][1] + rec[1][1] + rec[1][4]) {
			send[rec[0][0]] = rec[0][0];
		}
		else
			send[rec[0][0]] = rec[1][0];
	}
	if (rec[1][2] == 2) {
		if (rec[1][1] <= rec[1][4] + rec[0][1] + rec[0][4]) {
			send[rec[1][0]] = rec[1][0];
		}
		else
			send[rec[1][0]] = rec[0][0];
	}
	else {
		if (rec[1][4] <= rec[1][1] + rec[0][1] + rec[0][4]) {
			send[rec[1][0]] = rec[1][0];
		}
		else
			send[rec[1][0]] = rec[0][0];
	}
	if (rec[0][1] + rec[0][4] >= rec[1][1] + rec[1][4]) {
		if (rec[0][2] != 2) {
			send[rec[0][2]] = rec[1][0];
		}
		else
			send[rec[0][5]] = rec[1][0];
	}
	else {
		if (rec[0][2] != 2) {
			send[rec[0][2]] = rec[0][0];
		}
		else {
			send[rec[0][5]] = rec[0][0];
		}
	}
	return send;
}
/*�����˳��
0 1
2 3
�����ݴ�������ʱ������Ϊ9��
3->1����Ϊ0��
3->2����Ϊ1��
3->0��������㣻
*/
int *IP3(char rec[2][6]) {
	static int send[4];
	send[3] = 99;//0-self
	if (rec[0][2] == 3) {
		if (rec[0][1] <= rec[0][4] + rec[1][1] + rec[1][4]) {
			send[rec[0][0]] = rec[0][0];
		}
		else {
			send[rec[0][0]] = rec[1][0];
		}
	}
	else {
		if (rec[0][4] <= rec[0][1] + rec[1][1] + rec[1][4]) {
			send[rec[0][0]] = rec[0][0];
		}
		else
			send[rec[0][0]] = rec[1][0];
	}
	if (rec[1][2] == 3) {
		if (rec[1][1] <= rec[1][4] + rec[0][1] + rec[0][4]) {
			send[rec[1][0]] = rec[1][0];
		}
		else
			send[rec[1][0]] = rec[0][0];
	}
	else {
		if (rec[1][4] <= rec[1][1] + rec[0][1] + rec[0][4]) {
			send[rec[1][0]] = rec[1][0];
		}
		else
			send[rec[1][0]] = rec[0][0];
	}
	if (rec[0][1] + rec[0][4] >= rec[1][1] + rec[1][4]) {
		if (rec[0][2] != 3) {
			send[rec[0][2]] = rec[1][0];
		}
		else
			send[rec[0][5]] = rec[1][0];
	}
	else {
		if (rec[0][2] != 3) {
			send[rec[0][2]] = rec[0][0];
		}
		else {
			send[rec[0][5]] = rec[0][0];
		}
	}
	return send;
}
int *IP(int choice, char rec[2][6])
{
	int *send = NULL;
	switch (choice)
	{
	case 0:
		send = IP0(rec);
		break;
	case 1:
		send = IP1(rec);
		break;
	case 2:
		send = IP2(rec);
		break;
	case 3:
		send = IP3(rec);
		break;

	}
	return send;
}
void transferA(char a[], char b[]) {//a[6]-->b[12]
	char c[2];
	for (int i = 0; i < 6; i++) {
		acktobin(a[i], c);
		b[i * 2] = c[0];
		b[i * 2 + 1] = c[1];
	}

}
void transferB(char m[], char n[]) {//m[12]-->n[6]
	char d[2];
	int s = 1;
	for (int i = 0; i < 6; i++) {
		d[0] = m[i * 2];
		d[1] = m[i * 2 + 1];
		if (d[0] == 0 && d[1] == 0) {
			s = 0;
		}
		else if (d[0] == 0 && d[1] == 1) {
			s = 1;
		}
		else if (d[0] == 1 && d[1] == 0) {
			s = 2;
		}
		else if (d[0] == 1 && d[1] == 1) {
			s = 3;
		}
		n[i] = s;
		printf("%d", n[i]);
	}
}
#define MAX_BUFFSIZE 3600
#define SEND_L 3500
int main(int argc, char* argv[])
{
	SOCKET s;
	struct sockaddr_in remote_addr;
	char buf[MAX_BUFFSIZE] = { 0 };//���ջ���
	char sendbuf[MAX_BUFFSIZE] = { 0, 1, 1, 0, 1 };//��������
	WSAData wsa;
	int retval, selretval;
	struct socket_list sock_list;
	fd_set readfds, writefds, exceptfds;
	timeval timeout;
	fd_set read;
	unsigned long arg;
	int linecount = 0;
	unsigned short myPort;
	int ID;
	int i;

	/************************/
	int count = -1;
	int endline = 0;
	int length_restore = 3168;
	//char right_store[1000] = { 0 };
	int right_number = 0;
	char out[100];
	int k = -1;
	int K = 1;
	int flag = -1;
	int F;

	//char Pre_route[2][length_restoredata] = { 0 };
	//char Route[4] = { 5, 5, 5, 5 };
	/************************/

	WSAStartup(0x101, &wsa);

	//buf = (char *)malloc(MAX_BUFFSIZE);

	init_list(&sock_list);
	//�Ӽ������룬��Ҫ���ӵ�API�˿ں�
	printf("��ģ����Ӧ�ò�,\n");
	printf("�����뱾ģ����豸��ţ�");//��Ҫ�������ֲ�ͬ���豸
	scanf_s("%d", &ID);
	//�󶨱�ģ��˿ںţ��Խ���Ӧ�ò�����
	printf("�����ñ�ģ��Ķ˿ںţ�");
	scanf_s("%d", &myPort);

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == SOCKET_ERROR) {
		return 0;
	}

	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(myPort);
	remote_addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
	while (bind(s, (sockaddr*)&remote_addr, sizeof(remote_addr)) != 0) {
		printf("��ģ��Ķ˿ں��ѱ�ռ�ã��뻻һ������:");
		scanf_s("%d", &myPort);
		remote_addr.sin_port = htons(myPort);
	}
	sock_list.MainSock = s;

	printf("�����������ӵ������ģ��˿ںţ�");
	scanf_s("%d", &sock_list.MainPort);

	printf("�������������(С��20�ַ����ַ���):");
	printf("��ע�⣬�������������һ�ݲ�������\n");


	//�����׽��ֵ�״̬Ϊ����̬��Ҳ���Բ�����
	arg = 1;
	ioctlsocket(s, FIONBIO, &arg);

	timeout.tv_sec = 0; //���÷������ݵ�ʱ����Ϊ10΢��
	timeout.tv_usec = 10;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	/*	for(i = 0 ; i < 4000; i ++){
	sendbuf[i] = 1; //��ʼ������ȫΪ1,�Ա�����
	}
	*/
	/**************************/
	int receival;//���Ƿ���ܵ�֡
	int distinc;
	int back;
	int Seq = 0;
	int frame_length = 0;
	//int retval;//�������ݳ���
	//int s;//�׽���
	//���֡������
	char framebuff[30][3500] = { 0 };
	/********************************/
	FILE *fp = NULL;
	unsigned int imageSize, bitlong;
	char *imageBin;
	char *imageBase64;
	char *imagebit;
	char *imageOutput, *imageBase64_2;
	int rec_Imag_length = 0;
	int rec_bit_length = 0;
	char *right_store;
	char pro[30][3168] = { 0 };
	int j = 1;
	fp = fopen("C:\\Users\\hp\\Desktop\\123.jpg", "rb");   //������ͼƬ��ַ 
	fseek(fp, 0L, SEEK_END);
	imageSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	imageBin = (char *)malloc(sizeof(char)*imageSize);
	fread(imageBin, 1, imageSize, fp);
	fclose(fp);
	imageBase64 = (char *)malloc(sizeof(char)*imageSize * 2);
	imagebit = (char *)malloc(sizeof(char)*imageSize * 16);
	base64_encode(imageBin, imageBase64, imageSize);
	bitlong = base64tobit(imageBase64, imagebit, imageSize);
	int dest = 1, sour = 1;
	char binary_array[30][3168] = { 0 };//��ʼ����
	printf("������Ŀ�ĵ�ַ�ͷ��͵�ַ:\n");
	printf("dest=");
	scanf_s("%d", &dest);
	printf("sour=");
	scanf_s("%d", &sour);
	endline = Input(imagebit, imageSize, bitlong, dest, sour, binary_array);

	printf("endline:%d\n", endline);

	char crc_data[30][3500] = { 0 };
	char Crc_data[30][framelong_send] = { 0 };
	//����һ֡��
	char Crcend_line[framelong_send] = { 0 };
	char frameend_line[3500] = { 0 };
	CRC(binary_array[0], Crcend_line, p);
	make_frame(Crcend_line, frameend_line);
	//������֡
	for (i = 1; i <= endline; i++)
	{
		CRC(binary_array[i], Crc_data[i], p);
		make_frame(Crc_data[i], crc_data[i]);
	}
	for (i = 1; i <= endline; i++)
	{
		memcpy(framebuff[i], crc_data[i], 3500);
	}
	/**********************************/
	/*
	char ori[101] = { "abcdefghij0123456789~!@#$%^&*(ˮ�����������£���ǰ�˼�������)���ʦ" };
	char binary_array[32][32] = { 0 };
	endline = Input(ori, binary_array) + 1;
	printf("endline:%d\n", endline);
	char end_line[datalong_send] = { 0 };
	code(endline, end_line, 32);
	*/





	//����
	char arraybuff[4][64] = { 0 };
	int number = 0;//ָ��framebuff
	//ѭ�����б�־
	int Select = 0;
	int begin = 0;
	int finish = 3;
	//��������
	char restore_data[3500] = { 0 };
	//���Բ���
	int test;
	//��������
	char newdata[3500] = { 0 };
	char rawdata[MAX_BUFFSIZE] = { 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0 };
	//��ʼ������

	/***************************/
	/***************************/
	char Initial_route[4][6] = { { 0, 3, 1, 0, 3, 2 }, { 1, 3, 0, 1, 2, 3 }, { 2, 3, 0, 2, 1, 3 }, { 3, 2, 1, 3, 1, 2 } };
	char Route[2][6] = { 0 };
	char Send_route[3168] = { 0 };
	int port_host[4] = { 0 };
	int host1;
	int host2;
	int port_choice = 0;
	int flag_route = 0;
	int initial_choice = 0;
	printf("�����뱾���˿ںţ�");
	scanf_s("%d", &initial_choice);
	printf("�������뱾����Ӧ�˿�������������:\n");
	printf("0�Ŷ˿ڶ�Ӧ�����ţ�");
	scanf_s("%d", &host1);
	printf("1�Ŷ˿ڶ�Ӧ�����ţ�");
	scanf_s("%d", &host2);
	int HOST;
	printf("����Ŀ�������ţ�");
	scanf_s("%d", &HOST);
	port_host[host1] = 0;
	port_host[host2] = 1;
	for (i = 0; i < 4; i++)
	{
		if ((i != host1) && (i != host2))
		{
			port_host[i] = 100;
		}
	}
	int *send;
	int lll = 0;
	char route_crc[3173] = { 0 };
	char route_frame[3500] = { 0 };
	int MMM = 2;
	/***************************/
	while (1) {
		make_fdlist(&sock_list, &readfds);
		//�����̲����˻���select���ƣ����Ϸ��Ͳ������ݣ��ͽ��ղ������ݣ�Ҳ���Բ��ö��̣߳�һ��ר���ͣ�һ��ר���յķ���
		selretval = select(0, &readfds, &writefds, &exceptfds, &timeout);
		if (selretval == SOCKET_ERROR) {
			retval = WSAGetLastError();
			break;
		}
		/*����·����Ϣ*/
		if ((flag_route == 0) && (lll == 0))
		{
			printf("�����������������·�ɱ�\n");
			lll++;
		}
		while (MMM)
		{
			make_fdlist(&sock_list, &readfds);
			//�����̲����˻���select���ƣ����Ϸ��Ͳ������ݣ��ͽ��ղ������ݣ�Ҳ���Բ��ö��̣߳�һ��ר���ͣ�һ��ר���յķ���
			selretval = select(0, &readfds, &writefds, &exceptfds, &timeout);
			if (selretval == SOCKET_ERROR) {
				retval = WSAGetLastError();
				break;
			}
			if (flag_route<2)
			{
				if (FD_ISSET(sock_list.MainSock, &readfds)) {
					//��������յ�����
					retval = recv(sock_list.MainSock, buf, MAX_BUFFSIZE, 0);
					if (retval > 0)
					{
						for (i = 0; i < 12; i++)
						{
							Send_route[i] = buf[i];


						}
						transferB(Send_route, Route[flag_route]);
						flag_route++;
					}
				}
			}
			if (flag_route == 2)
			{
				printf("�Ѿ���������·�ɱ���ʼ���㲢����·�ɱ�");
				//char route[2][6] = { { 3,2,1,3,4,2 },{ 0,3,1,0,10,2 } };//��һ�б�ʾ��3->1����Ϊ2 ��3->2����Ϊ4

				//int i;
				send = IP(initial_choice, Route);
				printf("·�ɱ���Ϣ��\n");
				printf("Ŀ������  ��һվ����   ���������˿ں�\n");
				for (i = 0; i < 4; i++)
				{
					printf("%d          %d           %d\n", i, send[i], port_host[send[i]]);
				}
				flag_route++;
				MMM--;
			}

			if (_kbhit()) {

				_getch();
				printf("��ʼ����·��\n");
				remote_addr.sin_port = htons(sock_list.MainPort);
				s = sock_list.MainSock;
				transferA(Initial_route[initial_choice], Send_route);
				CRC(Send_route, route_crc, p);
				make_frame(route_crc, route_frame);
				retval = sendto(sock_list.MainSock, route_frame, SEND_L, 0, (sockaddr *)&remote_addr, sizeof(remote_addr));
				if (retval > 0)
					printf("�ѷ��ͳ�ʼ·�ɱ���ʼ�ȴ���������·�ɱ�\n");
				MMM--;

			}
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_ZERO(&exceptfds);
		}


		if (_kbhit()) {
			_getch();
			remote_addr.sin_port = htons(sock_list.MainPort);
			//����һ�ݲ������ݵ��ӿ�0
			/*
			�����ݵ�ʱ����Ҫ·�ɱ�
			��ȡGoal
			����˿�ѡ�����F
			ÿ�η�������֮ǰ�Ƚ�F�ŵ�sendbuf�ĵ�0λ
			retval = sendto(sock_list.MainSock, sendbuf, 5, 0, (sockaddr *)&remote_addr, sizeof(remote_addr));
			if (retval > 0)
			printf("����һ�ݲ�������\n");

			*/
			s = sock_list.MainSock;
			F = port_host[send[HOST]];
			Add(frameend_line, sendbuf, F);
			retval = sendto(sock_list.MainSock, sendbuf, SEND_L, 0, (sockaddr *)&remote_addr, sizeof(remote_addr));
			if (retval > 0)
				printf("send endline\n");
			while (K)
			{
				FD_ZERO(&readfds);

				FD_SET(sock_list.MainSock, &readfds);
				//����Selcet�ȴ���5s

				Select = select(s + 1, &readfds, NULL, NULL, NULL);

				//�ж��Ƿ�ɶ�
				if (FD_ISSET(s, &readfds) != 0)
				{
					//�������ݣ�retval=recv(s,rawdata,256,0);
					retval = recv(s, rawdata, MAX_BUFFSIZE, 0);
					//	retval = recv(s, buf, 256, 0);//recv���ص��ǽ������ݳ���
					//������ݷǿս��в���
					if (retval)
					{
						if (number < endline)
						{
							number++;
							//Seq++;
							//	Seq = Seq % 4;
							Add(framebuff[number], sendbuf, F);
							retval = sendto(sock_list.MainSock, sendbuf, SEND_L, 0, (sockaddr *)&remote_addr, sizeof(remote_addr));
							printf("sendone\n");
							printf("updated and send,number=%d", number);
							printf("\n");

							if (number >= (endline))
							{
								K = 0;
								printf("��������ѷ������\n");
							}
						}
					}
					//�޴�������
					//������ȷ���ͣ�����



				}

			}


		}

		if (flag_route >= 2)
		{
			if (FD_ISSET(sock_list.MainSock, &readfds)) {
				//��������յ�����
				retval = recv(sock_list.MainSock, buf, MAX_BUFFSIZE, 0);
				if (retval > 0) {
					printf("�յ�����\n");
					/*
					if(flag<2)
					{
					for(i=0;i<length_restoredata;i++)
					{
					Pre_route[flag][i]=buf[i];
					}
					flag++;
					}
					��·�ɱ�
					if(flag==2)
					{
					make_route(Pre_route,Route);
					flag++;
					}
					if(flag==3)
					{
					//��������
					if (count == -1)
					{
					uncode(buf, &endline);
					printf("send endline %d", endline);
					printf("\n");
					count++;
					}
					else
					{
					for (i = 0; i < length_restore; i++) {
					right_store[i + right_number] = buf[i];
					}
					right_number = right_number + i;
					endline--;
					if (endline == 0)
					{
					k = 0;
					}
					//��ack+1�󾭱����֡����;
					//send_ack(seq, &ACK, s);
					printf("send endline %d", endline);
					printf("\n");

					}

					}

					}
					if (k == 0)
					{
					tranlate(right_store, out);
					printf("���յ�������Ϣ��");
					for (int i = 0; i<100; i++) {
					printf("%c", out[i]);

					}
					k = -1;
					count = -1;
					}

					}
					*/
					if (count == -1)
					{
						//uncode(buf, &endline);
						rec_Imag_length = Output_imagelong(buf);
						rec_bit_length = Output_bitlong(buf);
						endline = rec_bit_length / 3160 + 1;
						printf("send endline %d", endline);
						printf("\n");
						count++;
					}
					else
					{
						for (i = 0; i < length_restore; i++) {
							pro[j][i] = buf[i];
						}
						j++;
						endline--;
						if (endline == 0)
						{
							k = 0;
						}
						//��ack+1�󾭱����֡����;
						//send_ack(seq, &ACK, s);
						printf("send endline %d", endline);
						printf("\n");

					}

				}

			}
			if (k == 0)
			{
				/*
				tranlate(right_store, out);
				printf("���յ�������Ϣ��");
				for (int i = 0; i<100; i++) {
				printf("%c", out[i]);

				}

				printf("hello world\n");
				system("pause");
				k = -1;
				count = -1;
				j = 1;
				*/
				right_store = (char *)malloc(sizeof(char)*rec_Imag_length * 16);
				imageOutput = (char *)malloc(sizeof(char)*rec_Imag_length);
				imageBase64_2 = (char *)malloc(sizeof(char)*rec_Imag_length * 2);
				Output(right_store, rec_bit_length, pro);
				//	tranlate(right_store, out);

				bittobase64(imageBase64_2, right_store, rec_Imag_length);
				base64_decode(imageBase64_2, imageOutput);

				//дͼ������ΪͼƬ��СimageSize��ͼƬԭ��imageOutput�����ΪͼƬ  
				fp = fopen("C:\\Users\\hp\\Desktop\\789.jpg", "wb");   //��д��ͼƬ��ַ 
				fwrite(imageOutput, 1, rec_Imag_length, fp);
				fclose(fp);
				printf("ͼƬ�ѽ������\n");
				k = -1;
				count = -1;
				j = 1;
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