/**
 * @file
 * vuo.event.allowChanges node implementation.
 *
 * @copyright Copyright © 2012–2016 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"

VuoModuleMetadata({
					  "title" : "Allow Changes",
					  "keywords" : [ "pulse", "watcher", "filter", "hold", "block", "prevent", "unchanged", "pass" ],
					  "version" : "1.1.0",
					  "genericTypes" : {
						  "VuoGenericType1" : {
							  "compatibleTypes" : [
								  /* Sync with vuo.event.changed */
								  "VuoBoolean", "VuoColor", "VuoImage", "VuoInteger",
								  "VuoPoint2d", "VuoPoint3d", "VuoPoint4d",
								  "VuoReal", "VuoScreen", "VuoText",
								  "VuoArtNetInputDevice", "VuoArtNetOutputDevice",
								  "VuoAudioInputDevice", "VuoAudioOutputDevice", "VuoAudioFrame", "VuoData",
								  "VuoCoordinateUnit", "VuoDistribution3d", "VuoDragEvent",
								  "VuoHidControl", "VuoHidDevice",
								  "VuoImageFormat",
								  "VuoFont", "VuoMidiController", "VuoMidiInputDevice",
								  "VuoMidiNote", "VuoMidiOutputDevice", "VuoMidiPitchBend",
								  "VuoMultisample", "VuoBaudRate", "VuoParity",
								  "VuoOscInputDevice", "VuoOscOutputDevice",
								  "VuoRelativeTime", "VuoRoundingMethod",
								  "VuoSerialDevice", "VuoVertexAttribute", "VuoSyphonServerDescription",
								  "VuoTempoRange", "VuoNumberFormat", "VuoIconPosition", "VuoMovieFormat",
								  "VuoVideoFrame", "VuoVideoInputDevice",
								  "VuoTime", "VuoTimeUnit", "VuoTimeFormat", "VuoWeekday",
								  "VuoHorizontalAlignment", "VuoVerticalAlignment", "VuoAnchor"
							  ]
						  }
					  },
					  "node": {
						  "exampleCompositions" : [ ]
					  }
				  });

VuoGenericType1 * nodeInstanceInit
(
	VuoInputData(VuoGenericType1) value
)
{
	VuoGenericType1 *lastValue = (VuoGenericType1 *)malloc(sizeof(VuoGenericType1));
	VuoRegister(lastValue, free);
	*lastValue = value;
	return lastValue;
}

void nodeInstanceEvent
(
		VuoInstanceData(VuoGenericType1 *) lastValue,
		VuoInputData(VuoGenericType1) value,
		VuoInputEvent({"eventBlocking":"door","data":"value"}) valueEvent,
		VuoOutputData(VuoGenericType1) changedValue,
		VuoOutputEvent({"data":"changedValue"}) changedValueEvent
)
{
	if (! VuoGenericType1_areEqual(**lastValue, value))
	{
		*changedValue = value;
		*changedValueEvent = true;
		**lastValue = value;
	}
}

void nodeInstanceFini
(
		VuoInstanceData(VuoGenericType1 *) lastValue
)
{
}
