/*
 * MFRenderContext.h
 *
 *  Created on: 02.09.2020
 *      Author: michl
 */

#ifndef MFRENDERCONTEXT_H_
#define MFRENDERCONTEXT_H_

#include "MFRenderLibPrinter.h"
class MFRenderContext {
public:
  static MFRenderContext
  *smp_staticContext;
  uint32_t
  m_print=0;
private:
  MFRenderLibPrinter
  *mp_printer=nullptr;
public:
  MFRenderContext(MFPrintTarget* pSubPrinter=nullptr);
  virtual ~MFRenderContext();
  MFRenderLibPrinter* getPrinter(){return mp_printer;}
  void printInfo(std::string info){
    MFObject::printInfo(info,m_print);
  }
  void printWarning(std::string warning){
    MFObject::printWarning(warning,m_print);
  }
  void printErr(std::string error){
    MFObject::printErr(error,m_print);
  }
};

#endif /* MFRENDERCONTEXT_H_ */
