#ifndef ASEMANGLOBALS_H
#define ASEMANGLOBALS_H

#include <QtGlobal>

#define SET_PROPERTY(PROPERTY_NAME) \
    if(_##PROPERTY_NAME == PROPERTY_NAME) return; \
    _##PROPERTY_NAME = PROPERTY_NAME; \
    Q_EMIT PROPERTY_NAME##Changed();

#define SET_PROPERTY_P(PROPERTY_NAME) \
    if(p->PROPERTY_NAME == PROPERTY_NAME) return; \
    p->PROPERTY_NAME = PROPERTY_NAME; \
    Q_EMIT PROPERTY_NAME##Changed();

#endif // ASEMANGLOBALS_H

