/*
 * MFShader.cpp
 *
 *  Created on: 15.07.2019
 *      Author: michl
 */

#include "MFShader.h"
#include <filesystem>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>

std::string MFShader::s_compilerPath="/home/michl/eclipse-workspace-cpp/MFRenderLibraries/glslangValidator";
std::string MFShader::s_outputPath="";
std::mutex* MFShader::s_pLockCompiler=new std::mutex();

bool MFShader::readFile(const std::string& fileName,std::vector<char> &buffer){
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
	  MFRenderContext::smp_staticContext->printErr("MFShader::readFile - Cant open shader file! " + fileName);
		return false;
	}

	try{
		size_t fileSize = (size_t) file.tellg();
			buffer.resize(fileSize);
			file.seekg(0);
			file.read(buffer.data(), fileSize);

			file.close();
	}catch(std::exception& e){
	  MFRenderContext::smp_staticContext->printErr("MFShader::readFile - failed to load data from file! Exception: "+std::string(e.what()));
		return false;
	}
	return true;
};

MFShader::MFShader(MFRenderContext* pContext):
	m_path(""),
	m_filename("")
{
  mp_context=pContext;
  if(mp_context==nullptr){
    mp_context=MFRenderContext::smp_staticContext;
  }
	m_isCompiled=false;
	m_isLoaded=false;
	m_useFile=false;
	s_pLockCompiler->lock();
		if(s_outputPath.length()==0){
			mp_context->printInfo("MFShader - setup of static output path");
			try{
				std::filesystem::path pOP="resources/shader/";
				mkdir("resources",0777);
				mkdir("resources/shader",0777);
				s_outputPath=std::filesystem::absolute(pOP);
	//				std::string lastChar=""+s_outputPath.at(s_outputPath.length()-1);
	//				if(lastChar.compare("/")!=0){
	//					mp_context->printInfo("MFShader - appending / to static shader output path!  "+lastChar+" in "+s_outputPath);
	//					s_outputPath+="/";
	//				}
			}catch(std::exception &e){
				mp_context->printWarning("MFShader - failed to make dir resources/shader - "+std::string(e.what()));
			}
		}

		m_outputPath=s_outputPath;
		mp_context->printInfo("MFShader - Output path: "+m_outputPath);
		s_pLockCompiler->unlock();
}
MFShader::MFShader(std::string path, std::string filename,MFRenderContext* pContext):
	m_path(""),
	m_filename("")
{
  mp_context=pContext;
  if(mp_context==nullptr){
    mp_context=MFRenderContext::smp_staticContext;
  }
	m_isCompiled=false;
	m_isLoaded=false;
	m_useFile=false;
	setInputFile(path, filename);
	s_pLockCompiler->lock();
	if(s_outputPath.length()==0){
		mp_context->printInfo("MFShader - setup of static output path");
		try{
			std::filesystem::path pOP="resources/shader/";
			mkdir("resources",0777);
			mkdir("resources/shader",0777);
			s_outputPath=std::filesystem::absolute(pOP);
//				std::string lastChar=""+s_outputPath.at(s_outputPath.length()-1);
//				if(lastChar.compare("/")!=0){
//					mp_context->printInfo("MFShader - appending / to static shader output path!  "+lastChar+" in "+s_outputPath);
//					s_outputPath+="/";
//				}
		}catch(std::exception &e){
			mp_context->printWarning("MFShader - failed to make dir resources/shader - "+std::string(e.what()));
		}
	}

	m_outputPath=s_outputPath;
	mp_context->printInfo("MFShader - added:"+m_path+m_filename+" Output path: "+m_outputPath);
	s_pLockCompiler->unlock();
}

MFShader::~MFShader() {
	// TODO Auto-generated destructor stub
}

void MFShader::setInputFile(std::string parentDir,std::string fileName){
	m_path=parentDir;
	m_filename=fileName;
}

bool MFShader::compile(){
	if(m_isCompiled){
		mp_context->printInfo("MFShader::compile - Shader compiled already! "+m_binaryFilePath);
		return true;
	}
	mp_context->printInfo("MFShader::compile() - Compiling shader!");

	/*Create output dir!*/
	mkdir(m_outputPath.data(),0777);

	std::string filename_split=m_filename.substr(0,m_filename.find("."));

	/*setup the compile command*/
	std::string compileCmd="cd "+MFShader::m_outputPath+"\n"+
			MFShader::s_compilerPath+
			" -o "+m_outputPath+filename_split+".spv"+
			" -V "+m_path+m_filename;

	std::system(compileCmd.data());

	m_isCompiled=std::filesystem::exists(m_outputPath+filename_split+".spv");
	if(m_isCompiled){
		m_binaryFilePath=m_outputPath+filename_split+".spv";
		mp_context->printInfo("MFShader::compile() - Compiling done! fileName:"+m_binaryFilePath);
	}else{
		mp_context->printInfo("MFShader::compile() - MFShader::compile() cmd failed:\n"+compileCmd+"\n"+
		    "Compiling failed! fileName:"+m_outputPath+filename_split+".spv");
		m_binaryFilePath="";
	}

	return m_isCompiled;
}

bool MFShader::updateBinaries(){
	if(m_useFile){
		return loadShaderFromFile(m_outputPath);
	}
	m_isCompiled=false;
	bool ret=compile();
	if(!ret){
		std::cout<<"updateBinaries failed!";
		return false;
	}
	try{
		if(!readFile(m_binaryFilePath,m_shaderBinary)){
		  mp_context->printErr("MFShader::updateBinaries - failed to read/load shader bindaries!");
		  return false;
		}
	}catch(const std::exception& e){
		return false;
	}
	return true;
}

bool MFShader::isCompiled(){
	return m_isCompiled;
}

void MFShader::setCompilerPath(std::string compilePath){
	s_pLockCompiler->lock();
	s_compilerPath=compilePath;
	s_pLockCompiler->unlock();
}

bool MFShader::loadShaderFromFile(std::string binaryPath){
	m_useFile=true;
	m_isLoaded=false;
	mp_context->printInfo("MFShader::loadShaderFromFile - Loading shader binaries from file: "+binaryPath);
	try{
		if(!readFile(binaryPath,m_shaderBinary)){
			mp_context->printErr("MFShader::loadShaderFromFile - Failed to load shader binaries from file: "+binaryPath);
			return false;
		}
	}catch(const std::exception& e){
		mp_context->printErr("MFShader::loadShaderFromFile - Failed to load shader binaries from file: "+binaryPath);
		return false;
	}
	mp_context->printInfo("MFShader::loadShaderFromFile - Loading shader binaries from file was successful! dataSize="+std::to_string(m_shaderBinary.size()));
	m_binaryFilePath=binaryPath;
	m_isLoaded=true;
	m_isCompiled=false;
	return true;
}

void MFShader::setOutputPath(std::string outputPath){
	m_outputPath=outputPath;
	std::filesystem::path pOP=m_outputPath;
	m_outputPath=std::filesystem::absolute(pOP);
	std::string lastChar=""+m_outputPath.at(m_outputPath.length()-1);
	if(lastChar!="/"){
		std::cout<<"MFShader::setOutputPath - appending / to shader output path! "+m_outputPath;
		m_outputPath+="/";
	}
}

std::vector<char>* MFShader::getShaderBinary(){
	if(m_isLoaded){
		return &m_shaderBinary;
	}
	if(!compile()){
		throw std::runtime_error("MFShader::getShaderBinary - compilation of shader failed!");
	}
	try{
		readFile(m_binaryFilePath,m_shaderBinary);
		m_isLoaded=true;
	}catch(const std::exception& e){
		mp_context->printErr("MFShader::getShaderBinary - failed!");
		return nullptr;
	}
	return &m_shaderBinary;
}

void MFShader::getOutputPath(std::string& outputPath){
	outputPath=m_outputPath;
}

void MFShader::setBaseOutputPath(std::string& outputPath){
	s_pLockCompiler->lock();
	s_outputPath=outputPath;
	s_pLockCompiler->unlock();
}

std::string MFShader::getBaseOutputPath(){
	std::string ret;
	s_pLockCompiler->lock();
	ret=s_outputPath;
	s_pLockCompiler->unlock();
	return ret;
}

