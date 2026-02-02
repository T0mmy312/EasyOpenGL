#ifndef GLA_PROGRAM_H
#define GLA_PROGRAM_H

#include <string>
#include <stdexcept>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

namespace gla {

class Shader;

/**
 * @brief Exception thrown when Program linking fails.
 */
class ProgramLinkError : public std::runtime_error {
public:
    /**
     * @brief Construct a new Program Link Error object.
     * 
     * @param infoLog InfoLog buffer from glGetProgramInfoLog.
     */
    ProgramLinkError(const std::string& infoLog)
        : std::runtime_error(
              "Program link failed:\n" + infoLog) {}
};

/**
 * @brief Exception thrown when Program validation fails, mainly used in debug builds.
 */
class ProgramValidateError : public std::runtime_error {
public:
    /**
     * @brief Construct a new Program Validate Error object.
     * 
     * @param infoLog InfoLog buffer from glGetProgramInfoLog.
     */
    ProgramValidateError(const std::string& infoLog)
        : std::runtime_error(
              "Program validation failed:\n" + infoLog) {}
};

struct UniformData {
    int location;
    unsigned int glType;
    int arraySize;
};

/**
 * @brief Program class to abstract OpenGL Shader Programs.
 * 
 * @warning Program must be deconstructed before the OpenGL context is destroyed.
 * @warning This class is not guaranteed to be thread-safe.
 * 
 * @note This class owns the underlying OpenGL Program object and
 *       releases it upon destruction or reset().
 */
class Program {
protected:
    unsigned int _id = 0;
    bool _linked = false;

    std::unordered_map<std::string, int> _uniformIndexMap = {}; // name to uniform index conversion
    std::unordered_map<int, int> _uniformLocationIndexMap = {}; // location to uniform index conversion
    std::vector<UniformData> _uniformData = {}; // uniform data per index

    void _delete();
    void _check() const;
    void _ensure() const;
    void _queryUniformData();
    void _setupUniform(int loc, int sizeCheck, int typeCheck) const;
    std::string _getError();

    class UniformProxy {
    private:
        Program& _parent;
        int _location;

    public:
        UniformProxy(Program& parent, int location) : _parent(parent), _location(location) {}
        UniformProxy(UniformProxy&& other) = delete;
        UniformProxy(const UniformProxy& other) = delete;

        explicit operator float() const { float val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::vec2() const { glm::vec2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::vec3() const { glm::vec3 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::vec4() const { glm::vec4 val; _parent.getUniform(_location, val); return val; }
        explicit operator int() const { int val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::ivec2() const { glm::ivec2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::ivec3() const { glm::ivec3 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::ivec4() const { glm::ivec4 val; _parent.getUniform(_location, val); return val; }
        explicit operator unsigned int() const { unsigned int val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::uvec2() const { glm::uvec2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::uvec3() const { glm::uvec3 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::uvec4() const { glm::uvec4 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat2() const {glm::mat2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat3() const {glm::mat3 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat4() const {glm::mat4 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat2x3() const {glm::mat2x3 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat3x2() const {glm::mat3x2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat2x4() const {glm::mat2x4 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat4x2() const {glm::mat4x2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat3x4() const {glm::mat3x4 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat4x3() const {glm::mat4x3 val; _parent.getUniform(_location, val); return val; }

        UniformProxy& operator=(float other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::vec2& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::vec3& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::vec4& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(int other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::ivec2& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::ivec3& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::ivec4& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(unsigned int other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::uvec2& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::uvec3& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::uvec4& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::mat2& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::mat3& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::mat4& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::mat2x3& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::mat3x2& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::mat2x4& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::mat4x2& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::mat3x4& other) { _parent.setUniform(_location, other); return *this; }
        UniformProxy& operator=(const glm::mat4x3& other) { _parent.setUniform(_location, other); return *this; }

        UniformProxy& operator=(const UniformProxy& other) = delete;
        UniformProxy& operator=(UniformProxy&& other) = delete;
    };

    class UniformProxyConst {
    private:
        const Program& _parent;
        int _location;

    public:
        UniformProxyConst(const Program& parent, int location) : _parent(parent), _location(location) {}
        UniformProxyConst(UniformProxyConst&& other) = delete;
        UniformProxyConst(const UniformProxyConst& other) = delete;

        explicit operator float() const { float val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::vec2() const { glm::vec2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::vec3() const { glm::vec3 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::vec4() const { glm::vec4 val; _parent.getUniform(_location, val); return val; }
        explicit operator int() const { int val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::ivec2() const { glm::ivec2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::ivec3() const { glm::ivec3 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::ivec4() const { glm::ivec4 val; _parent.getUniform(_location, val); return val; }
        explicit operator unsigned int() const { unsigned int val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::uvec2() const { glm::uvec2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::uvec3() const { glm::uvec3 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::uvec4() const { glm::uvec4 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat2() const {glm::mat2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat3() const {glm::mat3 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat4() const {glm::mat4 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat2x3() const {glm::mat2x3 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat3x2() const {glm::mat3x2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat2x4() const {glm::mat2x4 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat4x2() const {glm::mat4x2 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat3x4() const {glm::mat3x4 val; _parent.getUniform(_location, val); return val; }
        explicit operator glm::mat4x3() const {glm::mat4x3 val; _parent.getUniform(_location, val); return val; }

        UniformProxyConst& operator=(const UniformProxyConst& other) = delete;
        UniformProxyConst& operator=(UniformProxyConst&& other) = delete;
    };

public:
    /**
     * @brief Construct an empty Program object.
     */
    Program();
    Program(Program&& other);
    Program(const Program&) = delete; // OpenGL Programs are not copy safe
    ~Program();

    /**
     * @brief Resets the Program to an empty State.
     * 
     * @throws std::runtime_error If OpenGL failed to create a new Program.
     */
    void reset();

    /**
     * @brief Checks if the given Shader is attached to the Program.
     * 
     * @param shader The Shader to check.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * 
     * @return true if the Shader is attached, false otherwise.
     */
    bool attached(const Shader& shader);

    /**
     * @brief Attaches Shader to Program for linking.
     * 
     * @note Attaching a Shader invalidates the current link state.
     * 
     * @warning Attaching a destroyed Shader is undefined behavior.
     * @warning Attached Shaders must outlive the Program (at least until linking).
     * 
     * @throws std::runtime_error If the Shader has already been attached.
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * 
     * @param shader The Shader that should be attached to the Program.
     */
    void attach(const Shader& shader);

    /**
     * @brief Detaches Shader from Program for linking.
     * 
     * @note Detaching a Shader invalidates the current link state.
     * 
     * @throws std::runtime_error If the Shader was not attached.
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * 
     * @param shader The Shader that should be detached from the Program.
     */
    void detach(const Shader& shader);

    /**
     * @brief Gets if the Program has been successfully linked.
     * 
     * @return true if the Program is linked, false otherwise.
     */
    bool linked() const { return _linked; }

    /**
     * @brief Links all attached Shaders and creates a valid Program.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * @throws gla::ProgramLinkError If the Program fails to link. The Program remains in a valid state afterwards.
     * @throws gla::ProgramValidateError If the Program fails to validate. This only occurs when DEBUG_BUILD is defined else the validity is not checked.
     */
    void link();

    /**
     * @brief Binds this Program.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * @throws std::runtime_error If the Program was not linked to avoid invalid usage.
     */
    void bind() const;

    /**
     * @brief Unbinds a Program by binding id 0.
     */
    static void unbind();

    /**
     * @brief Gets the Location of the given Uniform.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * @throws std::runtime_error If the current Program was not linked before use
     * @throws std::invalid_argument If the uniform does not exist
     * 
     * @param name The name of the uniform to find
     * @returns The location of the uniform
     */
    int getUniformLocation(const std::string& name) const;

    /**
     * @brief Sets a uniform at the given location.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * @throws std::invalid_argument If the location does not correspond to a uniform
     * @throws std::runtime_error If the type does not correspond to the GLSL type
     * @throws std::invalid_argument If it attempts to set a GLSL array
     * @throws std::runtime_error If the current Program is unlinked
     * 
     * @warning Setting a uniform binds the program.
     * 
     * @param location The uniform location returned by getUniformLocation()
     * @param data The value to assign to the uniform
     *
     * This overload works for `float`, `int`, `glm::vec*`, `glm::mat*`, etc.
     */
    void setUniform(int location, float data);
    void setUniform(int location, const glm::vec2& data); ///< \overload
    void setUniform(int location, const glm::vec3& data); ///< \overload
    void setUniform(int location, const glm::vec4& data); ///< \overload
    void setUniform(int location, int data); ///< \overload
    void setUniform(int location, const glm::ivec2& data); ///< \overload
    void setUniform(int location, const glm::ivec3& data); ///< \overload
    void setUniform(int location, const glm::ivec4& data); ///< \overload
    void setUniform(int location, unsigned int data); ///< \overload
    void setUniform(int location, const glm::uvec2& data); ///< \overload
    void setUniform(int location, const glm::uvec3& data); ///< \overload
    void setUniform(int location, const glm::uvec4& data); ///< \overload
    void setUniform(int location, const glm::mat2& data); ///< \overload
    void setUniform(int location, const glm::mat3& data); ///< \overload
    void setUniform(int location, const glm::mat4& data); ///< \overload
    void setUniform(int location, const glm::mat2x3& data); ///< \overload
    void setUniform(int location, const glm::mat3x2& data); ///< \overload
    void setUniform(int location, const glm::mat2x4& data); ///< \overload
    void setUniform(int location, const glm::mat4x2& data); ///< \overload
    void setUniform(int location, const glm::mat3x4& data); ///< \overload
    void setUniform(int location, const glm::mat4x3& data); ///< \overload
    /**
     * @brief Sets a uniform at the given name.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * @throws std::invalid_argument If the location does not correspond to a uniform
     * @throws std::runtime_error If the type does not correspond to the GLSL type
     * @throws std::invalid_argument If it attempts to set a GLSL array
     * @throws std::runtime_error If the current Program is unlinked
     * 
     * @warning Setting a uniform binds the program.
     * 
     * @param name The uniform name
     * @param data The value to assign to the uniform
     *
     * This overload works for `float`, `int`, `glm::vec*`, `glm::mat*`, etc.
     */
    void setUniform(const std::string& name, float data) { setUniform(getUniformLocation(name), data); }
    void setUniform(const std::string& name, const glm::vec2& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::vec3& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::vec4& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, int data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::ivec2& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::ivec3& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::ivec4& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, unsigned int data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::uvec2& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::uvec3& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::uvec4& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::mat2& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::mat3& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::mat4& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::mat2x3& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::mat3x2& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::mat2x4& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::mat4x2& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::mat3x4& data) { setUniform(getUniformLocation(name), data); } ///< \overload
    void setUniform(const std::string& name, const glm::mat4x3& data) { setUniform(getUniformLocation(name), data); } ///< \overload

    /**
     * @brief Gets a uniform at the given location.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * @throws std::invalid_argument If the location does not correspond to a uniform
     * @throws std::runtime_error If the type does not correspond to the GLSL type
     * @throws std::invalid_argument If it attempts to get a GLSL array
     * @throws std::runtime_error If the current Program is unlinked
     * 
     * @param location The uniform location returned by getUniformLocation()
     * @param data The variable to store the data in
     *
     * This overload works for `float`, `int`, `glm::vec*`, `glm::mat*`, etc.
     */
    void getUniform(int location, float& data) const;
    void getUniform(int location, glm::vec2& data) const; ///< \overload
    void getUniform(int location, glm::vec3& data) const; ///< \overload
    void getUniform(int location, glm::vec4& data) const; ///< \overload
    void getUniform(int location, int& data) const; ///< \overload
    void getUniform(int location, glm::ivec2& data) const; ///< \overload
    void getUniform(int location, glm::ivec3& data) const; ///< \overload
    void getUniform(int location, glm::ivec4& data) const; ///< \overload
    void getUniform(int location, unsigned int& data) const; ///< \overload
    void getUniform(int location, glm::uvec2& data) const; ///< \overload
    void getUniform(int location, glm::uvec3& data) const; ///< \overload
    void getUniform(int location, glm::uvec4& data) const; ///< \overload
    void getUniform(int location, glm::mat2& data) const; ///< \overload
    void getUniform(int location, glm::mat3& data) const; ///< \overload
    void getUniform(int location, glm::mat4& data) const; ///< \overload
    void getUniform(int location, glm::mat2x3& data) const; ///< \overload
    void getUniform(int location, glm::mat3x2& data) const; ///< \overload
    void getUniform(int location, glm::mat2x4& data) const; ///< \overload
    void getUniform(int location, glm::mat4x2& data) const; ///< \overload
    void getUniform(int location, glm::mat3x4& data) const; ///< \overload
    void getUniform(int location, glm::mat4x3& data) const; ///< \overload
    /**
     * @brief Gets a uniform at the given name.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * @throws std::invalid_argument If the location does not correspond to a uniform
     * @throws std::runtime_error If the type does not correspond to the GLSL type
     * @throws std::invalid_argument If it attempts to get a GLSL array
     * @throws std::runtime_error If the current Program is unlinked
     * 
     * @param name The uniform name
     * @param data The variable to store the data in
     *
     * This overload works for `float`, `int`, `glm::vec*`, `glm::mat*`, etc.
     */
    void getUniform(const std::string& name, float& data) const { getUniform(getUniformLocation(name), data); }
    void getUniform(const std::string& name, glm::vec2& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::vec3& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::vec4& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, int& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::ivec2& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::ivec3& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::ivec4& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, unsigned int& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::uvec2& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::uvec3& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::uvec4& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::mat2& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::mat3& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::mat4& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::mat2x3& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::mat3x2& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::mat2x4& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::mat4x2& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::mat3x4& data) const { getUniform(getUniformLocation(name), data); } ///< \overload
    void getUniform(const std::string& name, glm::mat4x3& data) const { getUniform(getUniformLocation(name), data); } ///< \overload

    Program& operator=(Program&& other);
    Program& operator=(const Program&) = delete; // OpenGL Programs are not copy safe

    /**
     * @brief Returns a Proxy to get / set the uniform at the given location with assignment and type conversion operators.
     * 
     * @note Possible errors when assigning can be found under gla::Program::setUniform.
     * @note Possible errors on explicit type conversion can be found under gla::Program::getUniform.
     *  
     * @warning Setting a uniform binds the program.
     * 
     * @param location The uniform location returned by getUniformLocation()
     */
    UniformProxy operator[](int location) { return UniformProxy(*this, location); }
    /**
     * @brief Returns a Proxy to get / set the uniform with the given name with assignment and type conversion operators.
     * 
     * @note Possible errors when assigning can be found under gla::Program::setUniform.
     * @note Possible errors on explicit type conversion can be found under gla::Program::getUniform.
     * 
     * @warning Setting a uniform binds the program.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * @throws std::runtime_error If the current Program was not linked before use
     * @throws std::invalid_argument If the uniform does not exist
     *  
     * @param name The uniform name
     */
    UniformProxy operator[](const std::string& name) { return UniformProxy(*this, getUniformLocation(name)); }
    /**
     * @brief Returns a constant only Proxy to get the uniform at the given location with type conversion operators.
     * 
     * @note Possible errors on explicit type conversion can be found under gla::Program::getUniform.
     *  
     * @param location The uniform location returned by getUniformLocation()
     */
    UniformProxyConst operator[](int location) const { return UniformProxyConst(*this, location); }
    /**
     * @brief Returns a Proxy to get the uniform with the given name with type conversion operators.
     * 
     * @note Possible errors on explicit type conversion can be found under gla::Program::getUniform.
     * 
     * @throws std::logic_error If the current Program object does not exist (reset() is recommended to return to a valid state)
     * @throws std::runtime_error If the current Program was not linked before use
     * @throws std::invalid_argument If the uniform does not exist
     *  
     * @param name The uniform name
     */
    UniformProxyConst operator[](const std::string& name) const { return UniformProxyConst(*this, getUniformLocation(name)); }
};

}

#endif