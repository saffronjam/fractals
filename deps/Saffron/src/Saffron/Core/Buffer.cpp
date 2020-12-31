#include "SaffronPCH.h"

#include "Saffron/Core/Buffer.h"
#include "Saffron/Core/Assert.h"

namespace Se
{
Buffer::Buffer() : Buffer(nullptr, 0) {}

Buffer::Buffer(Uint8 *data, Uint32 size) : _data(data), _size(size) {}

Buffer Buffer::Copy(const void *data, Uint32 size)
{
	Buffer buffer;
	buffer.Allocate(size);
	memcpy(buffer._data, data, size);
	return buffer;
}

Buffer Buffer::Copy(const Buffer &buffer)
{
	return Copy(buffer.Data(), buffer.Size());
}

Buffer Buffer::Encapsulate(Uint8 *data)
{
	Buffer buffer;
	buffer._data = data;
	buffer._size = 0;
	return buffer;
}

void Buffer::Allocate(Uint32 size)
{
	if ( _data )
	{
		delete[] _data;
		_data = nullptr;
	}

	if ( size == 0 )
		return;

	_data = new Uint8[size];
	_size = size;
}

void Buffer::ZeroInitialize()
{
	if ( _data )
		memset(_data, 0, _size);
}

void Buffer::Write(void *data, Uint32 size, Uint32 offset) const
{
	SE_CORE_ASSERT(offset + size <= _size, "Buffer overflow");
	memcpy(_data + offset, data, size);
}

void Buffer::Write(const void *data, Uint32 size, Uint32 offset) const
{
	SE_CORE_ASSERT(offset + size <= _size, "Buffer overflow");
	memcpy(_data + offset, data, size);
}

void Buffer::Destroy()
{
	if ( _data )
	{
		delete[] _data;
	}
	Reset();
}

void Buffer::Reset()
{
	_data = nullptr;
	_size = 0;
}

Buffer::operator bool() const
{
	return _data;
}

Uint8 &Buffer::operator[](int index)
{
	return _data[index];
}

Uint8 Buffer::operator[](int index) const
{
	return _data[index];
}
}
