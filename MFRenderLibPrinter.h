/*
 * MFRenderLibPrinter.h
 *
 *  Created on: 26.08.2020
 *      Author: michl
 */

#ifndef MFRENDERLIBPRINTER_H_
#define MFRENDERLIBPRINTER_H_

#include <MFPrinters/MFPrintTarget.h>
#include <MFPrinters/MFPrinterStructs.h>
/**
 * This print target doesn't print anything. It shall be used to add own
 * sub print targets with MFPrintTarget::addSubTarget(...). The sub targets
 * MFPrintTarget::print() will be called if sth is printed in the MFRenderLib
 */
class MFRenderLibPrinter: public MFPrintTarget {
protected: /*virtual functions of MFPrintTarget*/
  /**
   * Virtual print method for data output. Make sure to use it on one thread or make it
   * threadsafe!
   * @param data
   * @return
   */
  virtual bool printData(S_MFPrintMessage* pMsg);

public:
  uint32_t
  m_printIndex;
  MFRenderLibPrinter();
  virtual ~MFRenderLibPrinter();
};

#endif /* MFRENDERLIBPRINTER_H_ */
