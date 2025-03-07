package com.google.chip.chiptool.clusterclient.clusterinteraction

import chip.devicecontroller.ChipTLVType.*
import org.json.JSONArray
import org.json.JSONObject
import java.util.Base64

class TLVToJsonConverter {
    // Update the convertStructToJson method to pass the TLV value
    fun convertStructToJson(struct: StructType): JSONObject {
        return JSONObject().apply {
            for (i in 0 until struct.size()) {
                val element = struct.value(i)
                val tagNum = element.contextTagNum()
                val value = element.value()

                put(createJsonKey(tagNum, value.type(), value), convertTLVToJsonValue(value))
            }
        }
    }

    private fun convertTLVToJsonValue(tlv: BaseTLVType): Any? {
        return when (tlv.type()) {
            TLVType.UInt -> {
                val uintValue = (tlv as UIntType).value()
                if (uintValue < Int.MAX_VALUE) uintValue.toInt() else uintValue.toString()
            }

            TLVType.Int -> {
                val intValue = (tlv as IntType).value()
                if (intValue in Int.MIN_VALUE..Int.MAX_VALUE) intValue.toInt() else intValue.toString()
            }

            TLVType.String -> (tlv as StringType).value()

            TLVType.ByteArray -> Base64.getEncoder().encodeToString((tlv as ByteArrayType).value())

            TLVType.Boolean -> (tlv as BooleanType).value()

            TLVType.Float -> {
                val floatValue = (tlv as FloatType).value()
                when {
                    floatValue.isInfinite() && floatValue > 0 -> "Infinity"
                    floatValue.isInfinite() && floatValue < 0 -> "-Infinity"
                    else -> floatValue
                }
            }

            TLVType.Double -> {
                val doubleValue = (tlv as DoubleType).value()
                when {
                    doubleValue.isInfinite() && doubleValue > 0 -> "Infinity"
                    doubleValue.isInfinite() && doubleValue < 0 -> "-Infinity"
                    else -> doubleValue
                }
            }

            TLVType.Null -> null

            TLVType.Struct -> convertStructToJson(tlv as StructType)

            TLVType.Array -> {
                val arrayTlv = tlv as ArrayType
                JSONArray().apply {
                    for (i in 0 until arrayTlv.size()) {
                        put(convertTLVToJsonValue(arrayTlv.value(i)))
                    }
                }
            }

            TLVType.Empty -> null
        }
    }

    private fun createJsonKey(tagNum: Long, type: TLVType, tlv: BaseTLVType? = null): String {
        val typeStr = when (type) {
            TLVType.Array -> {
                val arrayTlv = tlv as ArrayType
                if (arrayTlv.size() == 0) {
                    "ARRAY-?"
                } else {
                    // Get the type of the first element to determine sub-element type
                    val firstElement = arrayTlv.value(0)
                    "ARRAY-${getTypeString(firstElement.type())}"
                }
            }
            else -> getTypeString(type)
        }

        return "$tagNum:$typeStr"
    }

    private fun getTypeString(type: TLVType): String = when (type) {
        TLVType.UInt -> "UINT"
        TLVType.Int -> "INT"
        TLVType.String -> "STRING"
        TLVType.ByteArray -> "BYTES"
        TLVType.Boolean -> "BOOL"
        TLVType.Float -> "FLOAT"
        TLVType.Double -> "DOUBLE"
        TLVType.Null -> "NULL"
        TLVType.Struct -> "STRUCT"
        TLVType.Array -> "ARRAY"  // This should never be used directly
        TLVType.Empty -> "EMPTY"
    }
}