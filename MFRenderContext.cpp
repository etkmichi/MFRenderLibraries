/*
 * MFRenderContext.cpp
 *
 *  Created on: 02.09.2020
 *      Author: michl
 */

#include "MFRenderContext.h"
MFRenderContext* MFRenderContext::smp_staticContext=new MFRenderContext();
MFRenderContext::MFRenderContext(MFPrintTarget* pSubPrinter) {
  mp_printer=new MFRenderLibPrinter();
  mp_printer->addSubTarget(pSubPrinter);
  m_print=mp_printer->m_printIndex;
}

MFRenderContext::~MFRenderContext() {
  // TODO Auto-generated destructor stub
}

