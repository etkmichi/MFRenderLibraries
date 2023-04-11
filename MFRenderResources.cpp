/*
 * MFRenderResources.cpp
 *
 *  Created on: 27.08.2020
 *      Author: michl
 */

#include "MFRenderResources.h"

MFRenderResources::MFRenderResources(MFRenderContext* pContext){
  m_print=pContext->m_print;
  mp_context=pContext;
}
MFRenderResources::~MFRenderResources(){

}
