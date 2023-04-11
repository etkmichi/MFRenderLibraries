/*
 * MFVkDefaultShader.cpp
 *
 *  Created on: 19.09.2019
 *      Author: michl
 */

#include "MFVkDefaultShader.h"
#include <boost/system/error_code.hpp>
#include <boost/filesystem.hpp>

MFVkDefaultShader::MFVkDefaultShader(MFVkContext* pContext) {
	/*check for shader file, if not existent write shader to disk*/
	mp_context=pContext;
	mp_defaultFragmentShader=nullptr;
	mp_defaultVertexShader=nullptr;
}

bool MFVkDefaultShader::initDefaultShader(
		MFVkShader* pVertexShader,
		MFVkShader* pFragmentShader){
	try{
		std::ofstream vertexShaderFile;
		std::ofstream fragmentShaderFile;

		std::string vertexShaderPath=mp_context->getDataPath()+"RenderLibRes/shaders2/";
		std::string vertexShaderName="defaultVertexShader.vert";

		std::string fragShaderPath=mp_context->getDataPath()+"RenderLibRes/shaders2/";
		std::string fragShaderName="defaultFragmentShader.frag";

		boost::system::error_code ec;
		if(!boost::filesystem::create_directories(vertexShaderPath,ec)){
			mp_context->printWarning("MFVkDefaultShader - Cant create shader path! " + vertexShaderPath+" " + ec.message());
		}
		if(!boost::filesystem::create_directories(fragShaderPath,ec)){
			mp_context->printWarning("MFVkDefaultShader - Cant create shader path!" + fragShaderPath +" " +ec.message());
		}
		if(!m_loadFromFile){
			vertexShaderFile.open(vertexShaderPath+vertexShaderName);
			vertexShaderFile<<vertexShader;
			vertexShaderFile.close();

			fragmentShaderFile.open(fragShaderPath+fragShaderName);
			fragmentShaderFile<<fragmentShader;
			fragmentShaderFile.close();
		}else{
			mp_context->printInfo("MFVkDefaultShader::initDefaultShader(...) - "
					"Shaders will not be overwritten by default shader!!");
		}

		pVertexShader->setInputFile(vertexShaderPath,vertexShaderName);
		pVertexShader->setShaderStage(VK_SHADER_STAGE_VERTEX_BIT);

		pFragmentShader->setInputFile(fragShaderPath,fragShaderName);
		pFragmentShader->setShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT);
	}catch(std::exception& e){
		mp_context->printErr("Exception caught: "+std::string(e.what()));
		return false;
	}
	return true;
}
bool MFVkDefaultShader::initDefaultVertexShader(MFVkShader* pVertexShader){
	try{
		std::ofstream vertexShaderFile;

		std::string vertexShaderPath=mp_context->getDataPath()+"RenderLibRes/shaders2/";
		std::string vertexShaderName="defaultVertexShader.vert";

		boost::system::error_code ec;
		if(!boost::filesystem::create_directories(vertexShaderPath,ec)){
			mp_context->printWarning("MFVkDefaultShader - initDefaultVertexShader: "
					"Cant create shader path! " + vertexShaderPath+" " + ec.message());
		}
		if(!m_loadFromFile){
			vertexShaderFile.open(vertexShaderPath+vertexShaderName);
			vertexShaderFile<<vertexShader;
			vertexShaderFile.close();
		}else{
			mp_context->printInfo("MFVkDefaultShader::initDefaultVertexShader(...) - "
					"Shaders will not be overwritten by default shader!!");
		}

		pVertexShader->setInputFile(vertexShaderPath,vertexShaderName);
		pVertexShader->setShaderStage(VK_SHADER_STAGE_VERTEX_BIT);
	}catch(std::exception& e){
		mp_context->printErr("MFVkDefaultShader::initDefaultVertexShader - "
				"Exception caught: "+std::string(e.what()));
		return false;
	}
	return true;
}
bool MFVkDefaultShader::initDefaultFragmentShader(MFVkShader* pFragmentShader){
	try{
		std::ofstream fragmentShaderFile;

		std::string fragShaderPath=mp_context->getDataPath()+"RenderLibRes/shaders2/";
		std::string fragShaderName="defaultFragmentShader.frag";

		boost::system::error_code ec;
		if(!boost::filesystem::create_directories(fragShaderPath,ec)){
			mp_context->printWarning("MFVkDefaultShader::initDefaultFragmentShader - "
					"Cant create shader path!" + fragShaderPath +" " +ec.message());
		}
		if(!m_loadFromFile){
			fragmentShaderFile.open(fragShaderPath+fragShaderName);
			fragmentShaderFile<<fragmentShader;
			fragmentShaderFile.close();
		}else{
			mp_context->printInfo("MFVkDefaultShader::initDefaultFragmentShader(...) - "
					"Shaders will not be overwritten by default shader!!");
		}
		pFragmentShader->setInputFile(fragShaderPath,fragShaderName);
		pFragmentShader->setShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT);
	}catch(std::exception& e){
		mp_context->printErr("MFVkDefaultShader::initDefaultFragmentShader -"
				" Exception caught: "+std::string(e.what()));
		return false;
	}
	return true;
}
bool MFVkDefaultShader::initDefaultShader(){
	try{
		std::ofstream vertexShaderFile;
		std::ofstream fragmentShaderFile;

		std::string vertexShaderPath=mp_context->getDataPath()+"RenderLibRes/shaders2/";
		std::string vertexShaderName="defaultVertexShader.vert";

		std::string fragShaderPath=mp_context->getDataPath()+"RenderLibRes/shaders2/";
		std::string fragShaderName="defaultFragmentShader.frag";

		boost::system::error_code ec;
		if(!boost::filesystem::create_directories(vertexShaderPath,ec)){
			mp_context->printWarning("MFVkDefaultShader - Cant create shader path! " + vertexShaderPath+" " + ec.message());
		}
		if(!boost::filesystem::create_directories(fragShaderPath,ec)){
			mp_context->printWarning("MFVkDefaultShader - Cant create shader path!" + fragShaderPath +" " +ec.message());
		}
		if(!m_loadFromFile){
			vertexShaderFile.open(vertexShaderPath+vertexShaderName);
			vertexShaderFile<<vertexShader;
			vertexShaderFile.close();

			fragmentShaderFile.open(fragShaderPath+fragShaderName);
			fragmentShaderFile<<fragmentShader;
			fragmentShaderFile.close();
		}else{
			mp_context->printInfo("MFVkDefaultShader::initDefaultShader(...) - "
					"Shaders will not be overwritten by default shader!!");
		}
		if(mp_defaultVertexShader!=nullptr){
			delete mp_defaultVertexShader;
		}
		mp_defaultVertexShader=new MFVkShader(
				vertexShaderPath,vertexShaderName,mp_context);
		mp_defaultVertexShader->setShaderStage(VK_SHADER_STAGE_VERTEX_BIT);

		if(mp_defaultFragmentShader!=nullptr){
			delete mp_defaultFragmentShader;
		}
		mp_defaultFragmentShader=new MFVkShader(
				fragShaderPath,fragShaderName,mp_context);
		mp_defaultFragmentShader->setShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT);

	}catch(std::exception& e){
		mp_context->printErr("Exception caught: "+std::string(e.what()));
		return false;
	}
	return true;
}

MFVkDefaultShader::~MFVkDefaultShader() {
	if(mp_defaultFragmentShader!=nullptr){
		delete mp_defaultFragmentShader;
	}
	if(mp_defaultVertexShader!=nullptr){
		delete mp_defaultVertexShader;
	}
}

MFVkShader* MFVkDefaultShader::getVertexShader(){
	return mp_defaultVertexShader;
}
MFVkShader* MFVkDefaultShader::getFragmentShader(){
	return mp_defaultFragmentShader;
}
