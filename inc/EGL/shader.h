#ifndef EGL_SHADER_H
#define EGL_SHADER_H

#include <string>
#include <stdexcept>
#include <iosfwd> // std::istream forward-declared

namespace egl {

class Program;

/**
 * @brief ShaderType enum to indicate usage.
 */
enum class ShaderType {
    Fragment,       ///< A Shader that is intended to run on the programmable fragment processor.
    Vertex,         ///< A Shader that is intended to run on the programmable vertex processor.
    Geometry,       ///< A Shader that is intended to run on the programmable geometry processor.
    TessEvaluation, ///< A Shader that is intended to run on the programmable tessellation processor in the evaluation stage.
    TessControl,    ///< A Shader that is intended to run on the programmable tessellation processor in the control stage.
    Compute         ///< A Shader intended to run on the programmable compute processor.
};

/**
 * @brief Converts the ShaderType enum into a std::string.
 * 
 * @param type The ShaderType to convert to a std::string.
 * @return The name of the given ShaderType (if unknown "INVALID" is returned).
 */
constexpr std::string shaderTypeToString(ShaderType type);

/**
 * @brief Converts the ShaderType enum into an OpenGL enum.
 * 
 * @throws std::logic_error If the ShaderType is invalid.
 * 
 * @param type The ShaderType to convert to a std::string.
 * @return The resulting OpenGL enum.
 */
unsigned int toGLenum(ShaderType type);

/**
 * @brief Exception thrown when Shader compilation fails.
 */
class ShaderCompileError : public std::runtime_error {
public:
    /**
     * @brief Construct a new Shader Compile Error object.
     * 
     * @param ShaderType The type of Shader that failed to compile.
     * @param infoLog InfoLog buffer from glGetShaderInfoLog.
     */
    ShaderCompileError(ShaderType ShaderType,
                       const std::string& infoLog)
        : std::runtime_error(
              "Shader compile failed (" + shaderTypeToString(ShaderType) + "):\n" + infoLog) {}
};

/**
 * @brief Shader class to abstract OpenGL Shaders.
 * 
 * @warning Shader must be deconstructed before the OpenGL context is destroyed.
 * @warning This class is not guaranteed to be thread-safe.
 * 
 * @note This class owns the underlying OpenGL Shader object and
 *       releases it upon destruction or reset().
 */
class Shader {
protected:
    unsigned int _id = 0;
    ShaderType _type;
    bool _compiled = false;

    void _delete();
    void _check();
    void _ensure();
    std::string _getError();

public:
    Shader() = delete;
    /**
     * @brief Construct a new Shader object of given type.
     * 
     * @throws std::logic_error If the given ShaderType is invalid.
     * @throws std::runtime_error If OpenGL failed to create a Shader object.
     */
    Shader(ShaderType type);
    /**
     * @brief Constructs and compiles a new Shader object of given type.
     * 
     * @throws std::logic_error If the given ShaderType is invalid.
     * @throws std::runtime_error If OpenGL failed to create a Shader object.
     * @throws std::invalid_argument If the Shader source is NULL.
     * @throws egl::ShaderCompileError If the Shader fails to compile.
     */
    Shader(ShaderType type, const char* src);
    /**
     * @brief Constructs and compiles a new Shader object of given type.
     * 
     * @throws std::logic_error If the given ShaderType is invalid.
     * @throws std::runtime_error If OpenGL failed to create a Shader object.
     * @throws std::invalid_argument If the Shader source is NULL.
     * @throws egl::ShaderCompileError If the Shader fails to compile.
     */
    Shader(ShaderType type, const std::string& src);
    /**
     * @brief Constructs and compiles a new Shader object of given type.
     * 
     * @throws std::logic_error If the given ShaderType is invalid.
     * @throws std::runtime_error If OpenGL failed to create a Shader object.
     * @throws std::invalid_argument If the istream is not good().
     * @throws std::invalid_argument If in fails to read the Shader source.
     * @throws std::invalid_argument If the given input stream is invalid.
     * @throws std::invalid_argument If the Shader source is NULL.
     * @throws egl::ShaderCompileError If the Shader fails to compile.
     */
    Shader(ShaderType type, std::istream& in);
    /**
     * @brief Constructs and compiles a new Shader object of given type.
     * 
     * @throws std::logic_error If the given ShaderType is invalid.
     * @throws std::runtime_error If OpenGL failed to create a Shader object.
     * @throws std::invalid_argument If the istream is not good().
     * @throws std::invalid_argument If in fails to read the Shader source.
     * @throws std::invalid_argument If the given input stream is invalid.
     * @throws std::invalid_argument If the Shader source is NULL.
     * @throws egl::ShaderCompileError If the Shader fails to compile.
     */
    Shader(ShaderType type, std::istream&& in);

    Shader(Shader&& other);
    Shader(const Shader& other) = delete; // OpenGL Shaders are not copy safe
    ~Shader() noexcept;

    /**
     * @brief Resets the Shader to an empty state.
     * 
     * @note Reset creates an entirely new Shader object of the current type and
     *       therefore can be used to get a valid State as long as OpenGL doesn't fail to create a new Shader object.
     * 
     * @throws std::runtime_error If OpenGL failed to create a new Shader object.
     */
    void reset();

    /**
     * @brief Gets if the Shader has been successfully compiled.
     */
    bool compiled() const { return _compiled; }

    /**
     * @brief Get the type of the Shader. 
     */
    ShaderType getType() const { return _type; }
    
    /**
     * @brief Compiles the Shader with the given source.
     * 
     * @throws std::invalid_argument If the Shader source is NULL.
     * @throws std::logic_error If the current Shader object does not exist (reset() is recommended to return to a valid state).
     * @throws egl::ShaderCompileError If the Shader fails to compile.
     * 
     * @param src Null terminated c-style string to compile.
     */
    void compile(const char* src);

    /**
     * @brief Compiles the Shader with the given source input stream.
     * 
     * @note Reads the entire stream and compiles it.
     * 
     * @throws std::invalid_argument If the istream is not good().
     * @throws std::invalid_argument If in fails to read the Shader source.
     * @throws std::invalid_argument If the Shader source is NULL.
     * @throws std::logic_error If the current Shader object does not exist (reset() is recommended to return to a valid state).
     * @throws egl::ShaderCompileError If the Shader fails to compile.
     * 
     * @param in Input stream to compile.
     */
    void compile(std::istream& in);

    /**
     * @brief Compiles the Shader with the given source input stream.
     * 
     * @note Reads the entire stream and compiles it.
     * 
     * @throws std::invalid_argument If the istream is not good().
     * @throws std::invalid_argument If in fails to read the Shader source.
     * @throws std::invalid_argument If the Shader source is NULL.
     * @throws std::logic_error If the current Shader object does not exist (reset() is recommended to return to a valid state).
     * @throws egl::ShaderCompileError If the Shader fails to compile.
     * 
     * @param in Input stream to compile.
     */
    void compile(std::istream&& in);

    /**
     * @brief Compiles the Shader with the given source.
     * 
     * @throws std::invalid_argument If the given input stream is invalid.
     * @throws std::invalid_argument If the Shader source is NULL.
     * @throws std::logic_error If the current Shader object does not exist (reset() is recommended to return to a valid state).
     * @throws egl::ShaderCompileError If the Shader fails to compile.
     * 
     * @param str Source to compile.
     */
    void compile(const std::string& str);

    friend Program;

    Shader& operator=(Shader&& other);
    Shader& operator=(const Shader& other) = delete; // OpenGL Shaders are not copy safe
};

}

#endif