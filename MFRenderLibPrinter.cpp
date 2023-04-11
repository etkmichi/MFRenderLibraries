/*
 * MFRenderLibPrinter.cpp
 *
 *  Created on: 26.08.2020
 *      Author: michl
 */

#include "MFRenderLibPrinter.h"
#include <MFObject.h>
#include <MFPrinters/MFPrinterface.h>
MFRenderLibPrinter::MFRenderLibPrinter() {
  m_printIndex=0;
  MFPrinterface* pPrinter=static_cast<MFPrinterface*>(MFObject::pPrinter);
  m_printIndex=pPrinter->addPrintTarget(this);
}

MFRenderLibPrinter::~MFRenderLibPrinter() {
  // TODO Auto-generated destructor stub
}

bool MFRenderLibPrinter::printData(S_MFPrintMessage* pMsg){
//  if(getVecSubPrinters()->size()<1)
  printToConsol(pMsg->preFormattedMessage);
  return true;
};
