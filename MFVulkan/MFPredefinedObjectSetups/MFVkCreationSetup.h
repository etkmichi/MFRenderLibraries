/*
 * MFVkCreationSetup.h
 *
 *  Created on: 19.02.2020
 *      Author: michl
 */

#ifndef MFVULKAN_MFPREDEFINEDOBJECTSETUPS_MFVKCREATIONSETUP_H_
#define MFVULKAN_MFPREDEFINEDOBJECTSETUPS_MFVKCREATIONSETUP_H_

class MFVkCreationSetup {
public:
	bool
		m_useComplexVertex=true;
public:
	MFVkCreationSetup();
	virtual ~MFVkCreationSetup();
};

#endif /* MFVULKAN_MFPREDEFINEDOBJECTSETUPS_MFVKCREATIONSETUP_H_ */
