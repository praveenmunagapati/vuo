/**
 * @file
 * vuo.osc.message.get.4 node implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"
#include "VuoOscMessage.h"

VuoModuleMetadata({
					 "title" : "Get Message Values",
					 "keywords" : [ "address", "data" ],
					 "version" : "1.0.0",
					 "genericTypes": {
						  "VuoGenericType1" : {
							  "compatibleTypes": [ "VuoBoolean", "VuoInteger", "VuoReal", "VuoText" ]
						  },
						  "VuoGenericType2" : {
							  "compatibleTypes": [ "VuoBoolean", "VuoInteger", "VuoReal", "VuoText" ]
						  },
						  "VuoGenericType3" : {
							  "compatibleTypes": [ "VuoBoolean", "VuoInteger", "VuoReal", "VuoText" ]
						  },
						  "VuoGenericType4" : {
							  "compatibleTypes": [ "VuoBoolean", "VuoInteger", "VuoReal", "VuoText" ]
						  }
					 },
					 "node": {
						 "exampleCompositions": [ "ReceiveOsc.vuo" ]
					 }
				 });

void nodeEvent
(
		VuoInputData(VuoOscMessage) message,
		VuoOutputData(VuoText) address,
		VuoOutputData(VuoGenericType1) data1,
		VuoOutputData(VuoGenericType2) data2,
		VuoOutputData(VuoGenericType3) data3,
		VuoOutputData(VuoGenericType4) data4
)
{
	*address = message->address;

	int dataCount = VuoOscMessage_getDataCount(message);

	if (1 <= dataCount)
		*data1 = VuoGenericType1_valueFromJson(VuoOscMessage_getDataJson(message, 1));

	if (2 <= dataCount)
		*data2 = VuoGenericType2_valueFromJson(VuoOscMessage_getDataJson(message, 2));

	if (3 <= dataCount)
		*data3 = VuoGenericType3_valueFromJson(VuoOscMessage_getDataJson(message, 3));

	if (4 <= dataCount)
		*data4 = VuoGenericType4_valueFromJson(VuoOscMessage_getDataJson(message, 4));
}
