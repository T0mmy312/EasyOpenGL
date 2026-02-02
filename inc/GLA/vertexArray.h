#ifndef GLA_VERTEX_ARRAY_H
#define GLA_VERTEX_ARRAY_H

#include <vector>
#include <stdexcept>

#include <GLA/buffer.h>
#include <GLA/debug.h>

namespace gla {

enum class VertexAttribType {
    Byte,           ///< GL_BYTE
    UnsignedByte,   ///< GL_UNSIGNED_BYTE
    Short,          ///< GL_SHORT
    UnsignedShort,  ///< GL_UNSIGNED_SHORT
    Int,            ///< GL_INT
    UnsignedInt,    ///< GL_UNSIGNED_INT
    HalfFloat,      ///< GL_HALF_FLOAT
    Float,          ///< GL_FLOAT
    Double,         ///< GL_DOUBLE
    Fixed           ///< GL_FIXED
};

enum class VertexAttribInterp {
    Float,  ///< Interprets the vertex attribute as a float
    Integer ///< Interprets the vertex attribute as an interger
};


/**
 * @brief Converts a VertexAttribType into a GLenum.
 * 
 * @throws std::invalid_argument If the given VertexAttribType is invalid
 */
unsigned int toGLenum(VertexAttribType type);

/**
 * @brief Checks if the type and interpretation combination is valid.
 * 
 * @param error The error string output
 * 
 * @returns true if it is valid, false otherwise
 */
bool validateTypeInterpretation(VertexAttribType type, VertexAttribInterp interp, std::string& error);

/**
 * @brief Gets the size of the given type in bytes.
 * 
 * @throws std::invalid_argument If the given VertexAttribType is invalid
 */
int typeToBytes(VertexAttribType type);

/**
 * @brief Defines a vertex attribute for the gla::VertexArray.
 */
struct VertexAttribute {
    unsigned int index; ///< VertexAttribute location in the GLSL shader program.
    int numComponents; ///< number of components for VertexAttribute may be [1;4]. For example 3 for vec3. 
    VertexAttribType type; ///< Type of the VertexAttribute.
    VertexAttribInterp interp; ///< Interpretation of the VertexAttribute, for example is type Byte is specified, but should be used as a float
    bool normalized; ///< If it the vertex Attribute should be mapped to [-1;1] for signed values or [0;1] for unsigned values. (disregarded for int types)
    int offset; ///< Offset to the start of the current VertexAttribute
};

/**
 * @brief VertexArray class to abstract the OpenGL vertex array.
 * 
 * @warning Program must be deconstructed before the OpenGL context is destroyed.
 * @warning This class is not guaranteed to be thread-safe.
 * 
 * @note Inherits from gla::Buffer.
 */
class VertexArray : public Buffer {
private:
    std::vector<unsigned int> _enabledVertexAttribs = {};

public:
    VertexArray() : Buffer(BufferType::Array) {}
    VertexArray(VertexArray&& other) : Buffer(std::move(other)) {}
    VertexArray(const VertexArray& other) = delete;

    /**
     * @brief Set the Attributes for a vertex array.
     * 
     * @throws std::invalid_argument If stride is less than or equal to 0
     * @throws std::runtime_error If it could not query GL_MAX_VERTEX_ATTRIBS
     * @throws std::runtime_error If the current GPU doesn't support the given amount of VertexAttribute (at least 16 are guaranteed)
     * @throws std::invalid_argument If the any given index goes above the amount of VertexAttributes supported by the GPU (at least 16 are guaranteed)
     * @throws std::invalid_argument If any VertexAttribute requests less than 1 or more than 4 numComponents
     * @throws std::invalid_argument If any of the given combinations of type and interpretation is invalid
     * @throws std::invalid_argument If the given VertexAttribType in any VertexAttribute is invalid
     * 
     * @throws std::invalid_argument If the given VertexAttributes extend over the given stride (only when DEBUG_MODE is defined)
     * @throws std::invalid_argument If the given VertexAttributes overlap (only when DEBUG_MODE is defined)
     * 
     * @note Calling this function binds this Buffer.
     * 
     * @param attribs Vector of Attributes to assign to the VertexArray
     */
    void setAttributes(const std::vector<VertexAttribute>& attribs, int stride);

    VertexArray& operator=(VertexArray&& other) { Buffer::operator=(std::move(other)); return *this; }
    VertexArray& operator=(const VertexArray& other) = delete;
};

}

#endif