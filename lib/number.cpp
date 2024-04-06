#include "number.h"

int2023_t negative_value (const int2023_t& num) {
    int2023_t result;
    for (int32_t i = 0; i < kValueSize; i++) {
        result.value[i] = ~num.value[i];
    }

    return result;
}

int2023_t absolute_value(const int2023_t& num) {
    if (num.value[kValueSize - 1] < 128) {
        return num;
    } else {
        return negative_value(num);
    }
}

int2023_t from_int(int32_t i) {
    int2023_t converted_num;
    bool is_negative = (i < 0);
    if (is_negative) {
        i *= -1;
    }
    for(int32_t j = 0; j < 4; j++) {
        converted_num.value[j] = i & (kBase - 1);
        i = i >> 8;
    }
    if(is_negative) {
        converted_num = negative_value(converted_num);
    }

    return converted_num;
}

int2023_t from_string(const char* buff) {
    int2023_t converted_num;
    bool is_negative (buff[0] == '-');
    int32_t j = is_negative ? 1 : 0;
    for (int32_t i = j; buff[i] != '\0'; ++i) {
        converted_num = converted_num * from_int(10);
        converted_num = converted_num + from_int((buff[i] - '0'));
    }
    if (is_negative) {
        converted_num = negative_value(converted_num);
    }

    return converted_num;
}

int2023_t operator+(const int2023_t& lhs, const int2023_t& rhs) {
    bool is_lhs_negative = false;
    bool is_rhs_negative = false;
    if (lhs.value[kValueSize - 1] < 128) {
        is_lhs_negative = true;
    }
    if (rhs.value[kValueSize - 1] < 128) {
        is_rhs_negative = true;
    }
    if (absolute_value(lhs) == absolute_value(rhs) && is_rhs_negative != is_lhs_negative) {
        return from_int(0);
    }
    int32_t buffer = 0;
    int2023_t result = from_int(0);
    for (int32_t i = 0; i < kValueSize; i++) {
        int32_t pre_result;
        pre_result = lhs.value[i] + rhs.value[i] + buffer;
        result.value[i] = pre_result % kBase;
        buffer = pre_result / kBase;
    }
    if (buffer > 0) {
        result = result + from_int(1);
    }

    return result;
}

int2023_t operator-(const int2023_t& lhs, const int2023_t& rhs) {
    int2023_t result;
    int2023_t rhs_negative = negative_value(rhs);
    result = lhs + rhs_negative;

    return result;
}

int2023_t operator*(const int2023_t& lhs, const int2023_t& rhs) {
    int2023_t result = from_int(0);
    int32_t buffer = 0;
    int2023_t lhs_abs = absolute_value(lhs);
    int2023_t rhs_abs = absolute_value(rhs);
    for (int32_t i = 0; i < kValueSize; i++) {
        int2023_t pre_result = from_int(0);
        for (int32_t j = 0; j < kValueSize - i; j++) {
            int32_t temporary = lhs_abs.value[i] * rhs_abs.value[j] + buffer;
            buffer = temporary / kBase;
            pre_result.value[i + j] = temporary % kBase;
        }
        result = result + pre_result;
    }
    if (((lhs.value[kValueSize - 1] >= 128) != 0) && ((rhs.value[kValueSize - 1] >= 128) == 0)) {
        return negative_value(result);
    } else if (((lhs.value[kValueSize - 1] >= 128) == 0) && ((rhs.value[kValueSize - 1] >= 128) != 0)) {
        return negative_value(result);
    }

    return result;
}

int2023_t operator/(const int2023_t& lhs, const int2023_t& rhs) {
    int2023_t dividend = lhs;
    int2023_t divisor = rhs;
    int2023_t result;
    int2023_t pre_result;
    if (divisor == from_int(0)) {
        std::cerr << "UB";
        exit(1);
    }
    if (dividend == from_int(0)) {
        return from_int(0);
    }
    if (divisor == from_int(1)) {
        return dividend;
    }
    bool is_lhs_negative = (lhs.value[kValueSize - 1] >= 128);
    bool is_rhs_negative = (rhs.value[kValueSize - 1] >= 128);
    int2023_t dividend_abs = absolute_value(dividend);
    int2023_t divisor_abs = absolute_value(divisor);
    if (divisor_abs == dividend_abs) {
        if (is_lhs_negative && is_rhs_negative) {
            return from_int(1);
        } else if (!is_lhs_negative && !is_rhs_negative) {
            return from_int(1);
        } else if (!is_lhs_negative && is_rhs_negative) {
            return from_int(-1);
        } else if (is_lhs_negative && !is_rhs_negative) {
            return from_int(-1);
        }
    }
    int32_t i = kValueSize - 1;
    while (i >= 0) {
        pre_result = pre_result * from_int(256);
        pre_result.value[0] += dividend_abs.value[i];
        uint8_t j = 0;
        while (((divisor_abs * from_int(j)) <= pre_result)) {
            ++j;
        }
        --j;
        result.value[i] = j;
        pre_result = pre_result - (divisor_abs * from_int(j));
        --i;
    }
    if (!is_lhs_negative && is_rhs_negative && (result != from_int(0))) {
        return negative_value(result);
    } else if (is_lhs_negative && !is_rhs_negative && (result != from_int(0))) {
        return negative_value(result);
    }

    return result;
}

bool operator==(const int2023_t& lhs, const int2023_t& rhs) {
    bool is_lhs_negative = false;
    bool is_rhs_negative = false;
    if (lhs.value[kValueSize - 1] >= 128) {
        is_lhs_negative = true;
    }
    if (rhs.value[kValueSize - 1] >= 128) {
        is_rhs_negative = true;
    }
    if (is_lhs_negative != is_rhs_negative) {
        return false;
    }
    for (int i = 0; i < kValueSize; i++) {
        if (lhs.value[i] != rhs.value[i]) {
            return false;
        }
    }

    return true;
}

bool operator!=(const int2023_t& lhs, const int2023_t& rhs) {

    return !(lhs == rhs);
}

bool operator>(const int2023_t& lhs, const int2023_t& rhs) {
    if (((lhs.value[kValueSize - 1] >= 128) != 0) && ((rhs.value[kValueSize - 1] >= 128) == 0)) {
        return false;
    }
    if (((lhs.value[kValueSize - 1] >= 128) == 0) && ((rhs.value[kValueSize - 1] >= 128) != 0)) {
        return true;
    }
    for (int i = kValueSize - 2; i >= 0; --i) {
        if (lhs.value[i] != rhs.value[i]) {
            return lhs.value[i] > rhs.value[i];
        }
    }

    return false;
}

bool operator<(const int2023_t& lhs, const int2023_t& rhs) {
    if (((lhs.value[kValueSize - 1] >= 128) != 0) && ((rhs.value[kValueSize - 1] & 128) == 0)) {
        return true;
    }
    if (((lhs.value[kValueSize - 1] >= 128) == 0) && ((rhs.value[kValueSize - 1] & 128) != 0)) {
        return false;
    }
    for (int i = kValueSize - 2; i >= 0; --i) {
        if (lhs.value[i] != rhs.value[i]) {
            return lhs.value[i] < rhs.value[i];
        }
    }

    return false;
}

bool operator>=(const int2023_t& lhs, const int2023_t& rhs) {

    return (lhs == rhs) || (lhs > rhs);
}

bool operator<=(const int2023_t& lhs, const int2023_t& rhs) {

    return (lhs == rhs) || (lhs < rhs);
}

std::ostream& operator<<(std::ostream& stream, const int2023_t& value) {
    bool is_negative = false;
    if ((value.value[kValueSize - 1] >= 128) != 0) {
        is_negative = true;
    }
    if (is_negative) {
        stream << "-";
    }
    for (int32_t i = kValueSize - 1; i >= 0; --i) {
        stream << static_cast<int32_t>(value.value[i]);
    }

    return stream;
}
