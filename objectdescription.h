#ifndef OBJECTDESCRIPTION_H
#define OBJECTDESCRIPTION_H

struct objectDescription {
    double K = 0.0, S = 0.0, // K - Коэфицент формы, S - Площадь
    H = 0.0, Sat = 0.0, // H, Sat - цветовые компоненты модели HSL
    IMM = 0.0, D = 0.0, // IMM - отношение моментов инерции Макс \ Мин, D - сумма моментов инерции Макс + Мин
    ASM_R = 0.0, CON_R = 0.0, ENT_R = 0.0, LUN_R = 0.0, MPR_R = 0.0, // CON - Момент инерции, MPR - Максимальная вероятность
    ASM_G = 0.0, CON_G = 0.0, ENT_G = 0.0, LUN_G = 0.0, MPR_G = 0.0, // LUN - Локальная однородность, ENT - Энтропия
    ASM_B = 0.0, CON_B = 0.0, ENT_B = 0.0, LUN_B = 0.0, MPR_B = 0.0, // ASM - Энергия 
    ASM_L = 0.0, CON_L = 0.0, ENT_L = 0.0, LUN_L = 0.0, MPR_L = 0.0; // для модели RGB и яркости(L) 
    unsigned int type = 99;
    QString fileName;
};

#endif // OBJECTDESCRIPTION_H
