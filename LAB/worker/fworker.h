#ifndef SISTOPE_LAB2_1_2024_FWORKER_H
#define SISTOPE_LAB2_1_2024_FWORKER_H

#include "../estructuras.h"
#include "../filtros/filtros.h"

Worker pipeline(Worker *worker, int num_filters, int factor_saturacion, int umbral_binarizacion);

#endif // SISTOPE_LAB2_1_2024_FWORKER_H