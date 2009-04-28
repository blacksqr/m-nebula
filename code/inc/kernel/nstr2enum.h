#ifndef N_STR2ENUM_H
#define N_STR2ENUM_H
//--------------------------------------------------------------------
//  string to enum and enum to string template functions, by Rachel
//--------------------------------------------------------------------

/*
template for table with strings and enums
static struct {const char* str; enum en;} enum_table[] = {
	{"str0",			enum0},
	{"str1",			enum1},
    {"str2",			enum2},
	...

};
*/
template<typename ENM, typename TBL>
inline bool str2enum(const char *str, const TBL* pTable, unsigned int tablesize, ENM& e)
{	
	if (str && tablesize)
	{
		int num = tablesize / sizeof(pTable[0]); // sizeof(pTable[0] can't be 0 since tablesize > 0
		for(int i = 0; i < num; i++)
		{
			if (strcmp(pTable[i].str, str)==0)
			{
				e = pTable[i].en;
				return true;
			}
		}
		n_printf("invalid string %s for argument\n", str);
	}	
	return false;
}

template<typename ENM, typename TBL>
inline const char* enum2str(ENM e, const TBL* pTable, unsigned int tablesize)
{
    int num = 0;
	if(tablesize)
	{
		num = tablesize / sizeof(pTable[0]); // sizeof(pTable[0] can't be 0 since tablesize > 0
		for(int i = 0; i < num; i++)
		{
			if (pTable[i].en == e)
			{
				return pTable[i].str;
			}
		}
	}
    return 0;
}
#endif