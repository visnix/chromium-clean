// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file was generated by go/autofill-i18n-model-git. Do not manually edit.

#ifndef COMPONENTS_AUTOFILL_CORE_BROWSER_DATA_MODEL_AUTOFILL_I18N_FORMATTING_EXPRESSIONS_H_
#define COMPONENTS_AUTOFILL_CORE_BROWSER_DATA_MODEL_AUTOFILL_I18N_FORMATTING_EXPRESSIONS_H_

#include <string_view>

#include "base/containers/fixed_flat_map.h"
#include "components/autofill/core/browser/field_types.h"

namespace autofill {
namespace i18n_model_definition {

// A pair of country code and server field type used as keys in the
// `kAutofillFormattingRulesMap`.
using CountryAndFieldType = std::pair<std::string_view, ServerFieldType>;

// A lookup map for formatting expressions for countries and field types.
constexpr auto kAutofillFormattingRulesMap =
    base::MakeFixedFlatMap<CountryAndFieldType, std::u16string_view>({
      {{"BR", ADDRESS_HOME_ADDRESS}, u"${NAME_FULL;;}\n${ADDRESS_HOME_STREET_ADDRESS;;}\n${ADDRESS_HOME_DEPENDENT_LOCALITY;;}\n${ADDRESS_HOME_CITY;;} - ${ADDRESS_HOME_STATE;;}\n${ADDRESS_HOME_ZIP;;}"},
      {{"BR", NAME_FULL}, u"${NAME_FIRST;;} ${NAME_LAST;;}"},
      {{"BR", ADDRESS_HOME_STREET_ADDRESS}, u"${ADDRESS_HOME_STREET_LOCATION;;}\n${ADDRESS_HOME_OVERFLOW_AND_LANDMARK;;}"},
      {{"BR", ADDRESS_HOME_STREET_LOCATION}, u"${ADDRESS_HOME_STREET_NAME;;}, ${ADDRESS_HOME_HOUSE_NUMBER;;}"},
      {{"BR", ADDRESS_HOME_OVERFLOW_AND_LANDMARK}, u"${ADDRESS_HOME_OVERFLOW;;}\n${ADDRESS_HOME_LANDMARK;Ponto de referência: ;}"},
      {{"BR", ADDRESS_HOME_OVERFLOW}, u"${ADDRESS_HOME_SUBPREMISE;;}, ${COMPANY_NAME;;}"},
      {{"BR", ADDRESS_HOME_SUBPREMISE}, u"${ADDRESS_HOME_FLOOR;Andar ;}, ${ADDRESS_HOME_APT_NUM;;}"},
      {{"BR", ADDRESS_HOME_CITY}, u"${ADDRESS_HOME_DEPENDENT_LOCALITY;;}\n${ADDRESS_HOME_CITY;;}"},
      {{"BR", CREDIT_CARD_NAME_FULL}, u"${CREDIT_CARD_NAME_FIRST;;} ${CREDIT_CARD_NAME_LAST;;}"},
      {{"BR", CREDIT_CARD_EXP_DATE_2_DIGIT_YEAR}, u"${CREDIT_CARD_EXP_MONTH;;}/${CREDIT_CARD_EXP_2_DIGIT_YEAR;;}"},
      {{"BR", CREDIT_CARD_EXP_DATE_4_DIGIT_YEAR}, u"${CREDIT_CARD_EXP_MONTH;;}/${CREDIT_CARD_EXP_4_DIGIT_YEAR;;}"},
      {{"BR", PHONE_HOME_CITY_AND_NUMBER}, u"${PHONE_HOME_CITY_CODE_WITH_TRUNK_PREFIX;;}${PHONE_HOME_NUMBER;;}"},
      {{"MX", ADDRESS_HOME_ADDRESS}, u"${NAME_FULL;;}\n${ADDRESS_HOME_STREET_ADDRESS;;}\n${ADDRESS_HOME_DEPENDENT_LOCALITY;;}\n${ADDRESS_HOME_ZIP;;} ${ADDRESS_HOME_CITY;;}, ${ADDRESS_HOME_STATE;;}"},
      {{"MX", NAME_FULL}, u"${NAME_FIRST;;} ${NAME_MIDDLE;;} ${NAME_LAST;;}"},
      {{"MX", NAME_LAST}, u"${NAME_LAST_FIRST;;} ${NAME_LAST_CONJUNCTION;;} ${NAME_LAST_SECOND;;}"},
      {{"MX", ADDRESS_HOME_STREET_ADDRESS}, u"${ADDRESS_HOME_STREET_LOCATION;;}, ${ADDRESS_HOME_SUBPREMISE;;}\n${ADDRESS_HOME_OVERFLOW;;}"},
      {{"MX", ADDRESS_HOME_STREET_LOCATION}, u"${ADDRESS_HOME_STREET_NAME;;} ${ADDRESS_HOME_HOUSE_NUMBER;;}"},
      {{"MX", ADDRESS_HOME_SUBPREMISE}, u"${ADDRESS_HOME_APT_NUM;;}, ${ADDRESS_HOME_FLOOR;Piso ;}"},
      {{"MX", ADDRESS_HOME_OVERFLOW}, u"${COMPANY_NAME;;}\n${ADDRESS_HOME_BETWEEN_STREETS_OR_LANDMARK;;}\n${DELIVERY_INSTRUCTIONS;;}"},
      {{"MX", ADDRESS_HOME_BETWEEN_STREETS_OR_LANDMARK}, u"${ADDRESS_HOME_BETWEEN_STREETS;Entre Calles ;}\n${ADDRESS_HOME_LANDMARK;;}"},
      {{"MX", ADDRESS_HOME_BETWEEN_STREETS}, u"${ADDRESS_HOME_BETWEEN_STREETS_1;;} y ${ADDRESS_HOME_BETWEEN_STREETS_2;;}"},
      {{"MX", ADDRESS_HOME_CITY}, u"${ADDRESS_HOME_CITY;;}, ${ADDRESS_HOME_DEPENDENT_LOCALITY;;}"},
      {{"MX", ADDRESS_HOME_STATE}, u"${ADDRESS_HOME_STATE;;}, ${ADDRESS_HOME_ADMIN_LEVEL2;;}"},
      {{"MX", CREDIT_CARD_NAME_FULL}, u"${CREDIT_CARD_NAME_FIRST;;} ${CREDIT_CARD_NAME_LAST;;}"},
      {{"MX", CREDIT_CARD_EXP_DATE_2_DIGIT_YEAR}, u"${CREDIT_CARD_EXP_MONTH;;}/${CREDIT_CARD_EXP_2_DIGIT_YEAR;;}"},
      {{"MX", CREDIT_CARD_EXP_DATE_4_DIGIT_YEAR}, u"${CREDIT_CARD_EXP_MONTH;;}/${CREDIT_CARD_EXP_4_DIGIT_YEAR;;}"},
      {{"MX", PHONE_HOME_CITY_AND_NUMBER}, u"${PHONE_HOME_CITY_CODE_WITH_TRUNK_PREFIX;;}${PHONE_HOME_NUMBER;;}"},
      {{"US", ADDRESS_HOME_ADDRESS}, u"${NAME_FULL;;}\n${ADDRESS_HOME_STREET_ADDRESS;;}\n${ADDRESS_HOME_CITY;;}, ${ADDRESS_HOME_STATE;;} ${ADDRESS_HOME_ZIP;;}"},
      {{"US", NAME_FULL}, u"${NAME_FIRST;;} ${NAME_MIDDLE;;} ${NAME_LAST;;}"},
      {{"US", CREDIT_CARD_NAME_FULL}, u"${CREDIT_CARD_NAME_FIRST;;} ${CREDIT_CARD_NAME_LAST;;}"},
      {{"US", CREDIT_CARD_EXP_DATE_2_DIGIT_YEAR}, u"${CREDIT_CARD_EXP_MONTH;;}/${CREDIT_CARD_EXP_2_DIGIT_YEAR;;}"},
      {{"US", CREDIT_CARD_EXP_DATE_4_DIGIT_YEAR}, u"${CREDIT_CARD_EXP_MONTH;;}/${CREDIT_CARD_EXP_4_DIGIT_YEAR;;}"},
      {{"US", PHONE_HOME_CITY_AND_NUMBER}, u"${PHONE_HOME_CITY_CODE_WITH_TRUNK_PREFIX;;}${PHONE_HOME_NUMBER;;}"},
      {{"US", PHONE_HOME_NUMBER}, u"${PHONE_HOME_NUMBER_PREFIX;;}${PHONE_HOME_NUMBER_SUFFIX;;}"}
      });

}  // namespace i18n_model_definition
}  // namespace autofill

#endif  // COMPONENTS_AUTOFILL_CORE_BROWSER_DATA_MODEL_AUTOFILL_I18N_FORMATTING_EXPRESSIONS_H_
