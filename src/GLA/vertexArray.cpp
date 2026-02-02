#include <GLA/vertexArray.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace gla {

unsigned int toGLenum(VertexAttribType type) {
    switch (type)
    {
    case VertexAttribType::Byte:            return GL_BYTE;
    case VertexAttribType::UnsignedByte:    return GL_UNSIGNED_BYTE;
    case VertexAttribType::Short:           return GL_SHORT;
    case VertexAttribType::UnsignedShort:   return GL_UNSIGNED_SHORT;
    case VertexAttribType::Int:             return GL_INT;
    case VertexAttribType::UnsignedInt:     return GL_UNSIGNED_INT;
    case VertexAttribType::HalfFloat:       return GL_HALF_FLOAT;
    case VertexAttribType::Float:           return GL_FLOAT;
    case VertexAttribType::Double:          return GL_DOUBLE;
    case VertexAttribType::Fixed:           return GL_FIXED;
    }
    throw std::invalid_argument("Given VertexAttribType is invalid!");
}

bool validateTypeInterpretation(VertexAttribType type, VertexAttribInterp interp, std::string& error) {
    if (interp == VertexAttribInterp::Integer) {
        switch (type) {
        case VertexAttribType::HalfFloat: error = "Can't use type HalfFloat with Integer interpretation!"; return false;
        case VertexAttribType::Float: error = "Can't use type Float with Integer interpretation!"; return false;          
        case VertexAttribType::Double: error = "Can't use type Double with Integer interpretation!"; return false;         
        case VertexAttribType::Fixed: error = "Can't use type Fixed with Integer interpretation!"; return false;          
        }
    }
    return true;
}

int typeToBytes(VertexAttribType type) {
    switch (type)
    {
    case VertexAttribType::Byte:            return 1;
    case VertexAttribType::UnsignedByte:    return 1;
    case VertexAttribType::Short:           return 2;
    case VertexAttribType::UnsignedShort:   return 2;
    case VertexAttribType::Int:             return 4;
    case VertexAttribType::UnsignedInt:     return 4;
    case VertexAttribType::HalfFloat:       return 2;
    case VertexAttribType::Float:           return 4;
    case VertexAttribType::Double:          return 8;
    case VertexAttribType::Fixed:           return 4;
    }
    throw std::invalid_argument("Given VertexAttribType is invalid!");
}

// ----------------------------------------------------------------------------------------------------
// VertexArray class
// ----------------------------------------------------------------------------------------------------

// --------------------------------------------------
// public methods
// --------------------------------------------------

void VertexArray::setAttributes(const std::vector<VertexAttribute>& attribs, int stride) {
    if (stride <= 0)
        throw std::invalid_argument("stride must be greater than 0!");

    int maxVertexAttribs = -1;
    GL_CALL(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs));
    if (maxVertexAttribs < 0)
        throw std::runtime_error("Could not query GL_MAX_VERTEX_ATTRIBS!");
    if (maxVertexAttribs < attribs.size())
        throw std::runtime_error("The current GPU does not support " + std::to_string(attribs.size()) + " vertex attributes. Max allowed are: " + std::to_string(maxVertexAttribs) + ". At least 16 are guaranteed!");

    bind();

    for (unsigned int i : _enabledVertexAttribs)
        GL_CALL(glDisableVertexAttribArray(i));

    _enabledVertexAttribs.clear();
    _enabledVertexAttribs.reserve(attribs.size());

    DEBUG_ONLY(
    
    for (size_t i = 0; i < attribs.size(); i++) {
        size_t end = attribs[i].offset + typeToBytes(attribs[i].type) * attribs[i].numComponents;
        if (end > stride)
            throw std::invalid_argument("Given VertexAttributes are bigger than the stride!");
        for (size_t j = 0; j < attribs.size(); j++) {
            if (i == j) continue;
            if (attribs[i].offset <= attribs[j].offset && end > attribs[j].offset)
                throw std::invalid_argument("VertexAttributes " + std::to_string(i) + " and " + std::to_string(j) + " overlap!");
        }
    }

    )

    for (const VertexAttribute& attrib : attribs) {
        if (attrib.offset < 0)
            throw std::invalid_argument("Offset may not be less than 0!");
        if (attrib.index >= maxVertexAttribs)
            throw std::invalid_argument("The current GPU does not support indexes over " + std::to_string(attribs.size()) + "!");
        if (attrib.numComponents > 4 || attrib.numComponents <= 0)
            throw std::invalid_argument("numComponents of VertexAttribute may only be 1 to 4!");

        GL_CALL(glEnableVertexAttribArray(attrib.index));
        _enabledVertexAttribs.push_back(attrib.index);

        std::string error;
        if (!validateTypeInterpretation(attrib.type, attrib.interp, error))
            throw std::invalid_argument(error);
        
        if (attrib.interp == VertexAttribInterp::Integer)
            GL_CALL(glVertexAttribIPointer(attrib.index, attrib.numComponents, toGLenum(attrib.type), stride, (void*)attrib.offset));
        else
            GL_CALL(glVertexAttribPointer(attrib.index, attrib.numComponents, toGLenum(attrib.type), attrib.normalized, stride, (void*)attrib.offset));
    }
}

}