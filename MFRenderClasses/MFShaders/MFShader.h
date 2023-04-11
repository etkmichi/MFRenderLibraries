/*
 * MFShader.h
 *
 *  Created on: 15.07.2019
 *      Author: michl
 */

#ifndef MFRENDERCLASSES_MFSHADERS_MFSHADER_H_
#define MFRENDERCLASSES_MFSHADERS_MFSHADER_H_

#include <MFObjects/MFObject.h>
#include <MFThreadSystem/MFTaskThread.h>
#include <vector>
#include <mutex>
#include "../../MFRenderContext.h"

/**
 * This class represents shader. Shader can be compile after the compiler was setup with the given
 * functions (setCompilerPath(path)). If the shader is already compiled it can be loaded with
 * loadShaderFromFile(path). TODO automatically change compile shader and load...
 */
class MFShader {
protected:
	std::string
		m_path,
		m_filename,
		m_binaryFilePath,
		m_outputPath;

	bool
		m_isCompiled,
		m_isLoaded,
		m_useFile;

	static std::string
		s_compilerPath,
		s_outputPath;

	static std::mutex*
		s_pLockCompiler;

	MFRenderContext
	*mp_context;

	std::vector<char>
		m_shaderBinary;
public:
	MFShader(MFRenderContext* pContext);
	MFShader(std::string path, std::string filename, MFRenderContext* pContext);
	virtual ~MFShader();

	/**
	 * This method compiles the shader.
	 * @return
	 */
	bool compile();

	/**
	 * This function forces the recompilation of the shader.
	 * It will compile the file specified in the constructor.
	 *
	 * @return true if compilation was successful. false if compilation fails or file couldn't be read.
	 */
	bool updateBinaries();

	/**
	 * This function is used to load already compiled shader.
	 * Loads the shader with the specified file name (constructor or setFileName())
	 * from the output directory.
	 * If the file is doesn't exist, it will return false. The data can be retrieved with getShaderBinary().
	 * If data was loaded successfully, this function will set the isCompiled field to true.
	 * @return
	 */
	bool loadShaderFromFile(std::string binaryPath);

	bool isCompiled();

	/**
	 * This function returns the compiled shader data. If the shader wasn't compiled, this function call compile().
	 * @return the compiled shader data
	 */
	std::vector<char>* getShaderBinary();

	/**
	 * Sets the compile path. The path must contain the name of the compiler (vulkan sdk -> glslangValidator)
	 * @param compilePath
	 */
	static void setCompilerPath(std::string compilePath);

	/**
	 * Sets the output path for the compiled shaders.
	 * @param outputPath
	 */
	void setOutputPath(std::string outputPath);

	/**
	 * This function sets the input file which should be compiled.
	 * Sets the isLoaded field to false.
	 *
	 * @param parentDir
	 * @param fileName
	 */
	void setInputFile(std::string parentDir,std::string fileName);
	static void setBaseOutputPath(std::string& baseOutput);
	static std::string getBaseOutputPath();

	void getOutputPath(std::string& outputPath);

protected:
	static bool readFile(const std::string& fileName,std::vector<char> &buffer);
};

#endif /* MFRENDERCLASSES_MFSHADERS_MFSHADER_H_ */
