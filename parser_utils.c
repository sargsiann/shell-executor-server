#include "initials.h"

char	*str_realloc(char *old, char *to_add) 
{
	size_t	len = 0;
	char	*final = NULL;
	if (!*to_add)
		return old;
	if (!old)
	{
		final = strdup(to_add);
		return final;	
	}
	len += strlen(old);
	if (to_add != NULL)
		len += strlen(to_add);
	final = calloc(len + 1,sizeof(char));
	if (!final)
	{
		perror("Malloc Error");
		return NULL;
	}
	memcpy(final,old,strlen(old));
	if (to_add)
		memcpy(final + strlen(old),to_add,strlen(to_add));
	final[len] = 0;
	if (old != NULL)
		free(old);
	return final;
}

char	*substr(char *from, char *start, char *end) 
{
	char	*res;
	if (!from || !start || !end || start > end) {
		return NULL;
	}
	size_t len = end - start;
	res = calloc(len + 1, sizeof(char));
	size_t	i = 0;
	while (*start != *end)
	{	
		res[i] = *start;
		i++;
		start++;
	}
	res[i] = 0;
	return res;
}