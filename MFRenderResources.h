/*
 * MFRenderResources.h
 *
 *  Created on: 27.08.2020
 *      Author: michl
 */

#ifndef MFRENDERRESOURCES_H_
#define MFRENDERRESOURCES_H_
#include <MFObject.h>
#include <MFPrinters/MFPrinterface.h>
#include "MFRenderContext.h"
#include "MFRenderLibPrinter.h"

class MFRenderResources {
public:
  uint32_t
  m_print=0;
private:
  MFRenderContext
  *mp_context;
public:
  MFRenderResources(MFRenderContext* pContext);
  void printInfo(std::string info){
    MFObject::printInfo(info,m_print);
  }
  void printWarning(std::string warning){
    MFObject::printWarning(warning,m_print);
  }
  void printErr(std::string error){
    MFObject::printErr(error,m_print);
  }
  virtual ~MFRenderResources();
  MFRenderLibPrinter* getPrinter(){return mp_context->getPrinter();}

};



#endif /* MFRENDERRESOURCES_H_ */
