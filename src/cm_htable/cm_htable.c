// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   cm_htable.c                                        :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: rgramati <rgramati@student.42angouleme.fr  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/10/06 19:23:01 by rgramati          #+#    #+#             //
//   Updated: 2024/11/24 13:43:02 by rgramati         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include <stdlib.h>

#ifndef CM_HTABLE_IMPLEMENTATION
# define CM_HTABLE_IMPLEMENTATION
#endif
#include <cmem.h>

uint32_t	cm_djb2(const char *key)
{
	uint32_t	hash;

	hash = 5381;
	while (*key)
		hash = hash * 33 + *key++;
	return (hash);
}

t_cm_htable	*cm_htable_init(uint32_t capacity, void *ptr)
{
	struct s_cm_htable	*htable;

	if (!capacity)
		return (NULL);
	htable = ptr;
	if (!ptr)
		htable = malloc(sizeof(struct s_cm_htable));
	if (htable)
	{
		htable->size = 0;
		htable->capacity = cm_pow2next(capacity);
		htable->entries = (struct s_cm_entry *)
			((uint8_t *)htable) + sizeof(struct s_cm_htable);
		if (!ptr)
			htable->entries = malloc(htable->capacity * sizeof(struct s_cm_entry));
		if (!htable->entries)
		{
			free(htable);
			return (NULL);
		}
		cm_memset(htable->entries, 0, \
			CM_HTABLE_MAX_CAP * sizeof(struct s_cm_entry));
	}
	return (htable);
}

void	cm_htable_clear(t_cm_htable *htable_ptr, uint32_t flags)
{
	struct s_cm_htable	*htable;
	uint32_t			i;

	htable = (struct s_cm_htable *)htable_ptr;
	if (!htable)
		return ;
	if (flags & CM_CLEAR_FREE)
	{
		i = 0;
		while (i < htable->capacity)
			free(htable->entries[i++].key);
		if (!(flags & CM_CLEAR_STATIC))
		{
			free(htable->entries);
			free(htable);
		}
	}
	if (flags & CM_CLEAR_NULL && !(flags & CM_CLEAR_FREE))
		cm_memset(htable, 0, sizeof(uint32_t));
}

void	cm_htable_set(t_cm_htable *htable_ptr, const char *key, void *data)
{
	struct s_cm_htable	*htable;
	struct s_cm_entry	*entry;
	uint32_t			index;

	htable = (struct s_cm_htable *)htable_ptr;
	if (!htable)
		return ;
	// TODO: Handle table expanding
	index = cm_djb2(key) & (htable->capacity - 1);
	entry = &htable->entries[index];
	// if (htable->entries[index].key)
	// {
		// TODO: Handle collisions
	// }
	entry->key = cm_strdup(key);
	if (entry->data)
		htable->del(entry->data);
	htable->entries[index].data = data;
}

void	*cm_htable_get(t_cm_htable *htable_ptr, const char *key)
{
	struct s_cm_htable	*htable;
	struct s_cm_entry	*entry;
	uint32_t			index;

	htable = (struct s_cm_htable *)htable_ptr;
	if (!htable)
		return (NULL);
	index = cm_djb2(key) & (htable->capacity - 1);
	entry = &htable->entries[index];
	if (!entry->key || cm_strcmp(entry->key, key))
		return (NULL);
	return (entry->data);
}
