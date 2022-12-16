// stdafx.cpp : source file that includes just the standard includes
// PS3_PPU_Project.pch will be the pre-compiled header
// stdafx.o will contain the pre-compiled type information

#include "stdafx.h"


// std::lower_bound but without the bullshit
static menu_storage::menu_storage_pair* LowerBound(vector<menu_storage::menu_storage_pair>& data, int key)
{
	menu_storage::menu_storage_pair* first = data.Data;
	menu_storage::menu_storage_pair* last = data.Data + data.Size;
	size_t count = (size_t)(last - first);
	while (count > 0)
	{
		size_t count2 = count >> 1;
		menu_storage::menu_storage_pair* mid = first + count2;
		if (mid->key < key)
		{
			first = ++mid;
			count -= count2 + 1;
		}
		else
		{
			count = count2;
		}
	}
	return first;
}

// For quicker full rebuild of a storage (instead of an incremental one), you may add all your contents and then sort once.
void menu_storage::BuildSortByKey()
{
	struct StaticFunc
	{
		static int PairComparerByID(const void* lhs, const void* rhs)
		{
			// We can't just do a subtraction because qsort uses signed integers and subtracting our ID doesn't play well with that.
			if (((const menu_storage_pair*)lhs)->key > ((const menu_storage_pair*)rhs)->key) return +1;
			if (((const menu_storage_pair*)lhs)->key < ((const menu_storage_pair*)rhs)->key) return -1;
			return 0;
		}
	};
	std::qsort(Data.Data, (size_t)Data.Size, sizeof(menu_storage_pair), StaticFunc::PairComparerByID);
}

int menu_storage::GetInt(int key, int default_val) const
{
	menu_storage_pair* it = LowerBound(const_cast<vector<menu_storage_pair>&>(Data), key);
	if (it == Data.end() || it->key != key)
		return default_val;
	return it->val_i;
}

bool menu_storage::GetBool(int key, bool default_val) const
{
	return GetInt(key, default_val ? 1 : 0) != 0;
}

float menu_storage::GetFloat(int key, float default_val) const
{
	menu_storage_pair* it = LowerBound(const_cast<vector<menu_storage_pair>&>(Data), key);
	if (it == Data.end() || it->key != key)
		return default_val;
	return it->val_f;
}

void* menu_storage::GetVoidPtr(int key) const
{
	menu_storage_pair* it = LowerBound(const_cast<vector<menu_storage_pair>&>(Data), key);
	if (it == Data.end() || it->key != key)
		return NULL;
	return it->val_p;
}

// References are only valid until a new value is added to the storage. Calling a Set***() function or a Get***Ref() function invalidates the pointer.
int* menu_storage::GetIntRef(int key, int default_val)
{
	menu_storage_pair* it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
		it = Data.insert(it, menu_storage_pair(key, default_val));
	return &it->val_i;
}

bool* menu_storage::GetBoolRef(int key, bool default_val)
{
	return (bool*)GetIntRef(key, default_val ? 1 : 0);
}

float* menu_storage::GetFloatRef(int key, float default_val)
{
	menu_storage_pair* it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
		it = Data.insert(it, menu_storage_pair(key, default_val));
	return &it->val_f;
}

void** menu_storage::GetVoidPtrRef(int key, void* default_val)
{
	menu_storage_pair* it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
		it = Data.insert(it, menu_storage_pair(key, default_val));
	return &it->val_p;
}

// FIXME-OPT: Need a way to reuse the result of lower_bound when doing GetInt()/SetInt() - not too bad because it only happens on explicit interaction (maximum one a frame)
void menu_storage::SetInt(int key, int val)
{
	menu_storage_pair* it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
	{
		Data.insert(it, menu_storage_pair(key, val));
		return;
	}
	it->val_i = val;
}

void menu_storage::SetBool(int key, bool val)
{
	SetInt(key, val ? 1 : 0);
}

void menu_storage::SetFloat(int key, float val)
{
	menu_storage_pair* it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
	{
		Data.insert(it, menu_storage_pair(key, val));
		return;
	}
	it->val_f = val;
}

void menu_storage::SetVoidPtr(int key, void* val)
{
	menu_storage_pair* it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
	{
		Data.insert(it, menu_storage_pair(key, val));
		return;
	}
	it->val_p = val;
}

void menu_storage::SetAllInt(int v)
{
	for (int i = 0; i < Data.Size; i++)
		Data[i].val_i = v;
}

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
