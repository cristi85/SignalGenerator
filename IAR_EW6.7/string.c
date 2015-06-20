#include "types.h"

/* 
Function name:
    string_len
Parameters:
    src - Pointer to the string
Return:
    u8 - Length of string (until '\0' is reached)
*/
u8 string_len(const char* src)
{
  u8 i = 0;
  while(*(src++)) i++;
  return i;
}

/* 
Function name:
    string_copy
Description:
    Copy contents of src string to dest string
Parameters:
    dest - Pointer to destination string
    src  - Pointer to source string
Return:
    char* - Pointer to destination string
*/
char* string_copy(char* dest, const char* src)
{
  while(*(src)) *(dest++) = *(src++);
  *dest = 0;
  return dest;
}

/* 
Function name:
    string_append
Description:
    Append to dest string the contents of src string
Parameters:
    dest - Pointer to destination string
    src  - Pointer to source string
Return:
    char* - Pointer to destination string
*/
char* string_append(char* dest, const char* src)
{
  while(*(dest)) dest++;
  while(*(src)) *(dest++) = *(src++);
  *dest = 0;
  return dest;
}

/* 
Function name:
    string_substr
Description:
    Get substring of scr in dest
Parameters:
    dest - Pointer to destination string
    src  - Pointer to source string
Return:
    void
*/
char* string_substr(char* dest, const char* src, u8 startindex, u8 endindex)
{
  u8 cnt;
  if(startindex >= endindex) return dest;
  cnt = endindex - startindex;
  src += startindex;
  while(*(src) && cnt--)
  {
    *dest = *(src++);
  }
  return dest;
}

/* 
Function name:
    string_U32ToStr
Description:
    Convert number to string
Parameters:
    number - Number to be converted
    dest    - Pointer to string which will hold the result of conversion
Return:
    u8 - String length
*/
char* string_U32ToStr(u32 number, char* dest) //dest len=11 for max u32
{
  u32 tmp = number;
  u8 i = 9, j =0, k = 0;
  while(tmp)
  {
    dest[i--] = tmp%10+'0';
    tmp /= 10;
    j++;
  }
  for(;j;j--, k++)
  {
    dest[k] = dest[k+i+1];
  }
  dest[k] = 0;
  return dest;
}