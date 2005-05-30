/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */ 
/*
 * Module dedicated to base 64 Mime coding decoding
 *
 * Author: P. Cheyrou-lagreze (INRIA)
 *
 */
#ifdef _BASE64

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "appdialogue.h"
#include "application.h"


/* This digit string is used by MIME's base-64 encoding */
/* MIME also deliberately ignores `=' characters */
#define BASE64DIGITS \
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

#define EOS '\0'


static unsigned long BitSave = 0;
static int BitsSum = 0;
static int eof = 0;
static int count = 0;


#ifdef O

/* Masks for 0-8 bits */
static int mask[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

/*----------------------------------------------------------
    ReadBit: 
----------------------------------------------------------*/
static unsigned int ReadBit (int n,
		       int *pBitsRead,
		       char **input)
{
    unsigned long scratch;
    int c;
    
    while ((BitsSum < n) && (!eof))
      {
        c = **input++;
        if (c == EOF)
	  eof = 1;
        else
	  {
            BitSave <<= 8;
            BitSave |= (c & 0xff);
            BitsSum += 8;
	  }
      }
    if (BitsSum < n)
      {
        scratch = BitSave << (n - BitsSum);
        *pBitsRead = BitsSum;
        BitsSum = 0;
      }
    else
      {
        scratch = BitSave >> (BitsSum - n);
        *pBitsRead = n;
        BitsSum -= n;
      }
    return scratch & mask[n];
}
/*----------------------------------------------------------
    FillEncoded: pad output for MIME base64 encoding
----------------------------------------------------------*/
static void FillEncoded(char **output)
{

  while ((count % 4) != 0)
    {
      **output++ = '=';
      count++;
    }
}
/*----------------------------------------------------------
    Base64Encode: 
----------------------------------------------------------*/
static char *Base64Encode (char *input)
{
    int BitsSum = 6; /* Encode 6 bits at a time */
    int digit;
    char *output;
    const char *digits = BASE64DIGITS;
    int size = 0;
    
    output = TtaGetMemory (strlen (input)*2 + 1);
    digit = ReadBit (BitsSum, &BitsSum, &input);
    while (BitsSum > 0)
      {
	/* Encode extra bits at the end */
        *output++ = digits[digit];
	size++;
	
        digit = ReadBit (BitsSum, &BitsSum, &input);
      }
    /* Pad to multiple of four characters */
    FillEncoded (&output);
    /*output -> size*/
    return output;    
}
#endif /*O*/

/*-------------------------------------------------------------
    WriteBit:
----------------------------------------------------------*/
static char *WriteBit (unsigned bits, int n, char *output, int *size)
{
    BitSave = (BitSave << n) | bits;
    BitsSum += n;
    while (BitsSum > 7)
      {
        unsigned scratch = BitSave >> (BitsSum - 8);
        *output++ = scratch & 0xff;
	(*size)++;	
        BitsSum -= 8;
      }
    return output;    
}

/*----------------------------------------------------------
    ReadChar: Get next non-control character return EOF at
        end-of-file
----------------------------------------------------------*/
static char *ReadChar(int *c, char *input)
{  
  do
    {
      *c = *input++;
      if (*c == EOS)
	return input;
    }
  while ( (((*c+1) & 0x7f) < 33) ); /* No control character */
  return input;
}

/*----------------------------------------------------------
    Base64Decode: 
----------------------------------------------------------*/
static char *Base64Decode(char *input, int *size_back)
{
  const char *digits = BASE64DIGITS;
  int decode[256];
  char *output, *output2,*output2_base, *output_base;
  int i, c, digit;
  int size = 0;

  output = TtaGetMemory (strlen (input)+1);
  output_base = output;
  /* Build decode table */
  for (i=0;i<256;i++)
    decode[i] = -2; /* char Illegal */
  for (i=0;i<64;i++)
    {
      decode[(int) digits[i]] = i;
      decode[(int) (digits[i]|0x80)] = i; /* Ignore parity when decoding */
    }
  decode[(int)('=')] = -1;
  decode[(int)('='|0x80)] = -1; /* Ignore '=' for MIME */
  decode[32] = -1; /* Space problem with Illustrator..*/
  input = ReadChar (&c, input); 
  while (c != EOS)
    {
      digit = decode[c & 0x7f];
      if (digit < -1)
	{
	  fprintf (stderr,"Character not supported in 64 basis: %c\n", c);
	  exit (1);
	}
      else
	if (digit >= 0)
	  output = WriteBit (digit & 0x3f, 6, output, &size);
      input = ReadChar (&c, input);
    }
  
  output2 = TtaGetMemory (size);
  output2_base = output2;
  output = output - size;
  c = 0;  
  while (c < size)
    {
      *output2++ = *output++;
      c++;
    }
  TtaFreeMemory (output_base);
  *size_back = size;  
  return output2_base;
}
/*----------------------------------------------------------
   MakeImageFromBase64 : Make file based on a base 64 string
beginning with  "data:image/jpeg;base64," then data...
----------------------------------------------------------*/
int MakeImageFromBase64 (char *imagename)
{
  char     *output, *input;
  char     fname[4096];
  char     *Thot_Dir;
  FILE     *file;
  int      size, i;
  static   unsigned int uniquename=0;
  
  input = imagename;
  while (*input++ != ':')
    input++;
  
  
  if (strncasecmp (input, "image", 5))
    /*       "image/jpeg" Not mandatory ? 
      (don't seems to at least in illustrator SVG exporter)
      return 0;   */
    input += 1;/* ";" */
  else
    input += 6; /* "image;" */
  Thot_Dir = TtaGetEnvString ("THOTDIR");
  strcpy (fname, Thot_Dir);
  strcat (fname, "/config/test%i.");
  sprintf (fname, fname, uniquename++);
  i = 0;  
  while (*(input+i) != ';')
    i++;
  
  if (strncasecmp (input, "base64", 6))
    strncat (fname, input, i);  
  else
    /*illustrator doesn't give image type...*/
    strcat (fname, "\0");

  file = TtaWriteOpen (fname);
  if (file == NULL)
     {
	fprintf (stderr, "cannot open any file on your system %s\n", fname);
	return 0;
     }
  
  while (*input++ != ',')
    ;
  
  BitSave = 0;
  BitsSum = 0;
  eof = 0;
  count = 0;
  output = Base64Decode(input, &size);  
  fwrite (output, sizeof (char), size, file);  
  TtaWriteClose (file);
  strcpy (imagename, "file://");
  strcat (imagename, fname);
  return 1;
}
#endif /*_BASE64*/
