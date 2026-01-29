#include <EGL/buffer.h>

#include <EGL/debug.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace egl {

unsigned int toGLenum(BufferType type) {
    switch (type)
    {
    case BufferType::Array: return GL_ARRAY_BUFFER;
    case BufferType::AtomicCounter: return GL_ATOMIC_COUNTER_BUFFER;
    case BufferType::CopyRead: return GL_COPY_READ_BUFFER;
    case BufferType::CopyWrite: return GL_COPY_WRITE_BUFFER;
    case BufferType::DispatchIndirect: return GL_DISPATCH_INDIRECT_BUFFER;
    case BufferType::DrawIndirect: return GL_DRAW_INDIRECT_BUFFER;
    case BufferType::ElementArray: return GL_ELEMENT_ARRAY_BUFFER;
    case BufferType::PixelPack: return GL_PIXEL_PACK_BUFFER;
    case BufferType::PixelUnpack: return GL_PIXEL_UNPACK_BUFFER;
    case BufferType::Query: return GL_QUERY_BUFFER;
    case BufferType::ShaderStorage: return GL_SHADER_STORAGE_BUFFER;
    case BufferType::Texture: return GL_TEXTURE_BUFFER;
    case BufferType::TransformFeedback: return GL_TRANSFORM_FEEDBACK_BUFFER;
    case BufferType::Uniform: return GL_UNIFORM_BUFFER;
    }
    throw std::invalid_argument("BufferType is invalid!");
}

unsigned int toGLenum(BufferUsage usage) {
    switch (usage)
    {
    case BufferUsage::StreamDraw: return GL_STREAM_DRAW;
    case BufferUsage::StreamRead: return GL_STREAM_READ;
    case BufferUsage::StreamCopy: return GL_STREAM_COPY;
    case BufferUsage::StaticDraw: return GL_STATIC_DRAW;
    case BufferUsage::StaticRead: return GL_STATIC_READ;
    case BufferUsage::StaticCopy: return GL_STATIC_COPY;
    case BufferUsage::DynamicDraw: return GL_DYNAMIC_DRAW;
    case BufferUsage::DynamicRead: return GL_DYNAMIC_READ;
    case BufferUsage::DynamicCopy: return GL_DYNAMIC_COPY;
    }
    throw std::invalid_argument("BufferUsage is invalid!");
}

unsigned int toGLenum(MapUsage usage) {
    unsigned int flags = 0;
    auto check = [&](MapUsage m, GLenum f) {
        if ((usage & m) != MapUsage::None)
            flags |= f;
    };
    check(MapUsage::Read, GL_MAP_READ_BIT);
    check(MapUsage::Write, GL_MAP_WRITE_BIT);
    check(MapUsage::Persistent, GL_MAP_PERSISTENT_BIT);
    check(MapUsage::Coherent, GL_MAP_COHERENT_BIT);
    check(MapUsage::InvalidRange, GL_MAP_INVALIDATE_RANGE_BIT);
    check(MapUsage::InvalidateBuffer, GL_MAP_INVALIDATE_BUFFER_BIT);
    check(MapUsage::FlushExplicit, GL_MAP_FLUSH_EXPLICIT_BIT);
    check(MapUsage::Unsynchronized, GL_MAP_UNSYNCHRONIZED_BIT);
    return flags;
}

unsigned int toGLenum(BufferFlag flag) {
    unsigned int flags = 0;
    auto check = [&](BufferFlag m, GLenum f) {
        if ((flag & m) != BufferFlag::None)
            flags |= f;
    };
    check(BufferFlag::DynamicStorage, GL_DYNAMIC_STORAGE_BIT);
    check(BufferFlag::MapRead, GL_MAP_READ_BIT);
    check(BufferFlag::MapWrite, GL_MAP_WRITE_BIT);
    check(BufferFlag::MapPersistent, GL_MAP_PERSISTENT_BIT);
    check(BufferFlag::MapCoherent, GL_MAP_COHERENT_BIT);
    check(BufferFlag::ClientStorage, GL_CLIENT_STORAGE_BIT);
    return flags;
}

bool validateMapUsage(MapUsage usage, std::string& error) { // returns true if valid
    if ((usage & (MapUsage::Read | MapUsage::Write)) == MapUsage::None) {
        error = "Neither MapUsage::Read nor MapUsage::Write where defined!";
        return false;
    }
    if ((usage & (MapUsage::InvalidRange | MapUsage::Read)) == (MapUsage::InvalidRange | MapUsage::Read)) {
        error = "MapUsage::InvalidRange can't be used with MapUsage::Read!";
        return false;
    }
    if ((usage & (MapUsage::InvalidateBuffer | MapUsage::Read)) == (MapUsage::InvalidateBuffer | MapUsage::Read)) {
        error = "MapUsage::InvalidateBuffer can't be used with MapUsage::Read!";
        return false;
    }
    if ((usage & (MapUsage::Unsynchronized | MapUsage::Read)) == (MapUsage::Unsynchronized | MapUsage::Read)) {
        error = "MapUsage::Unsynchronized can't be used with MapUsage::Read!";
        return false;
    }
    if ((usage & (MapUsage::FlushExplicit | MapUsage::Write)) == MapUsage::FlushExplicit) {
        error = "MapUsage::Write must be set when using MapUsage::FlushExplicit!";
        return false;
    }
    return true;
}

bool validateBufferFlag(BufferFlag flag, std::string& error) {
    if ((flag & BufferFlag::MapPersistent) != BufferFlag::None && (flag & (BufferFlag::MapRead | BufferFlag::MapWrite)) == BufferFlag::None) {
        error = "BufferFlag::MapRead or BufferFlag::MapWrite must be set when using BufferFlag::MapPersistent!";
        return false;
    }
    if ((flag & BufferFlag::MapCoherent) != BufferFlag::None && (flag & BufferFlag::MapPersistent) == BufferFlag::None) {
        error = "BufferFlag::MapPersistent must be set when using BufferFlag::MapCoherent!";
        return false;
    }
    return true;
}

// ----------------------------------------------------------------------------------------------------
// class Buffer
// ----------------------------------------------------------------------------------------------------

// --------------------------------------------------
// protected methods
// --------------------------------------------------

void Buffer::_delete() {
    if (_id != 0)
        GL_CALL(glDeleteBuffers(1, &_id));
    _id = 0; 
}

void Buffer::_check() {
    if (_id == 0)
        throw std::runtime_error("Failed to create buffer object!");
}

// --------------------------------------------------
// constructors / destructors
// --------------------------------------------------

Buffer::Buffer(BufferType type) : _type(type) {
    GL_CALL(glGenBuffers(1, &_id));
    _check();
}
Buffer::Buffer(Buffer&& other)
    : _id(other._id), _mapped(other._mapped), _mapUsage(other._mapUsage), _type(other._type), _flags(other._flags) {
    other._id = 0;
    other._mapUsage = MapUsage::None;
    other._mapped = false;
    other._flags = BufferFlag::None;
}
Buffer::~Buffer() noexcept {
    _delete();
}

// --------------------------------------------------
// public methods
// --------------------------------------------------

void Buffer::bind() const {
    GL_CALL(glBindBuffer(toGLenum(_type), _id)); 
}

int64_t Buffer::size() const {
    bind();
    GLint64 size = 0;
    GL_CALL(glGetBufferParameteri64v(toGLenum(_type), GL_BUFFER_SIZE, &size));
    return size;
}

BufferType Buffer::getType() const {
    return _type;
}

void Buffer::setData(int64_t size, const void* data, BufferUsage usage) {
    bind();
    if (size < 0)
        throw std::runtime_error("size may not be negative!");
    _flags = BufferFlag::None;
    GL_CALL(glBufferData(toGLenum(_type), size, data, toGLenum(usage)));
}

void Buffer::setStorage(int64_t size, const void* data, BufferFlag flags) {
    bind();
    if (size <= 0)
        throw std::runtime_error("size must be greater than 0!");
    std::string error;
    if (!validateBufferFlag(flags, error))
        throw std::runtime_error("Invalid Buffer Flags:\n" + error);
    _flags = flags;
    GL_CALL(glBufferStorage(toGLenum(_type), size, data, toGLenum(flags)));
}

void Buffer::setSubData(int64_t offset, int64_t size, const void* data) {
    if (offset < 0)
        throw std::runtime_error("offset may not be negative!");
    if (size < 0)
        throw std::runtime_error("size may not be negative!");
    if (size + offset > Buffer::size())
        throw std::runtime_error("length + offset may not be greater than size()!");
    if (_mapped && (_mapUsage & MapUsage::Persistent) == MapUsage::None)
        throw std::runtime_error("setSubData can't be used when Buffer is mapped and MapUsage::Persistent is not set!");
    bind();
    GL_CALL(glBufferSubData(toGLenum(_type), offset, size, data));
}

void Buffer::getSubData(int64_t offset, int64_t size, void* data) {
    if (offset < 0)
        throw std::runtime_error("offset may not be negative!");
    if (size < 0)
        throw std::runtime_error("size may not be negative!");
    if (size + offset > Buffer::size())
        throw std::runtime_error("length + offset may not be greater than size()!");
    if (_mapped && (_mapUsage & MapUsage::Persistent) == MapUsage::None)
        throw std::runtime_error("getSubData can't be used when Buffer is mapped and MapUsage::Persistent is not set!");
    bind();
    GL_CALL(glGetBufferSubData(toGLenum(_type), offset, size, data));
}

void* Buffer::map(int64_t offset, int64_t length, MapUsage access) {
    if (_mapped)
        throw std::runtime_error("Buffer is already mapped!");
    if (length <= 0)
        throw std::runtime_error("length must be greater than 0!");
    if (offset < 0)
        throw std::runtime_error("offset may not be negative!");
    if (length + offset > size())
        throw std::runtime_error("length + offset may not be greater than size()!");
    std::string error;
    if (!validateMapUsage(access, error))
        throw std::runtime_error("Invalid map usage:\n" + error);
    if ((access & MapUsage::Persistent) != MapUsage::None && (_flags & BufferFlag::MapPersistent) == BufferFlag::None)
        throw std::runtime_error("MapUsage::Persistent requires BufferFlag::MapPersistent to be set through setStorage!");
    bind();
    void* ptr;
    GL_CALL(ptr = glMapBufferRange(toGLenum(_type), offset, length, toGLenum(access)));
    if (!ptr) throw std::runtime_error("glMapBufferRange returned nullptr");
    _mapped = true;
    _mapUsage = access;
    return ptr;
}

void Buffer::unmap() {
    bind();
    bool ok;
    GL_CALL(ok = glUnmapBuffer(toGLenum(_type)));
    if (ok == GL_FALSE) {
        _mapped = false;
        _mapUsage = MapUsage::None;
        throw std::runtime_error("glUnmapBuffer signalled data corruption");
    }
    _mapped = false;
    _mapUsage = MapUsage::None;
}

// --------------------------------------------------
// operator overloads
// --------------------------------------------------

Buffer& Buffer::operator=(Buffer&& other) {
    if (this != &other) {
        _delete();
        _id = other._id;
        _type = other._type;
        _mapped = other._mapped;
        _flags = other._flags;
        other._id = 0;
        other._mapped = false;
        other._flags = BufferFlag::None;
    }
    return *this;
}

}