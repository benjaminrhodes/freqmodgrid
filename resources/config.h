#define PLUG_NAME "FreqmodGrid"
#define PLUG_MFR "FreqMod"
#define PLUG_VERSION_HEX 0x00010000
#define PLUG_VERSION_STR "1.0.0"
#define PLUG_UNIQUE_ID 'FREQ'
#define PLUG_MFR_ID 'Freq'
#define PLUG_URL_STR "https://freqmod.com"
#define PLUG_EMAIL_STR "support@freqmod.com"
#define PLUG_COPYRIGHT_STR "Copyright 2026 FreqMod"
#define PLUG_CLASS_NAME FreqmodGrid

#define BUNDLE_NAME "FreqmodGrid"
#define BUNDLE_MFR "FreqMod"
#define BUNDLE_DOMAIN "com"

#define PLUG_CHANNEL_IO "0-2"
#define SHARED_RESOURCES_SUBPATH "FreqmodGrid"

#define PLUG_LATENCY 0
#define PLUG_TYPE 1
#define PLUG_DOES_MIDI_IN 1
#define PLUG_DOES_MIDI_OUT 0
#define PLUG_DOES_MPE 0
#define PLUG_DOES_STATE_CHUNKS 0
#define PLUG_HAS_UI 0
#define PLUG_WIDTH 400
#define PLUG_HEIGHT 300
#define PLUG_FPS 60
#define PLUG_SHARED_RESOURCES 0
#define PLUG_HOST_RESIZE 0

#define AUV2_ENTRY FreqmodGrid_Entry
#define AUV2_ENTRY_STR "FreqmodGrid_Entry"
#define AUV2_FACTORY FreqmodGrid_Factory
#define AUV2_VIEW_CLASS FreqmodGrid_View
#define AUV2_VIEW_CLASS_STR "FreqmodGrid_View"

#define AAX_TYPE_IDS 'FGR1', 'FGR2'
#define AAX_PLUG_MFR_STR "FreqMod"
#define AAX_PLUG_NAME_STR "FreqmodGrid\nFREQ"
#define AAX_DOES_AUDIOSUITE 0
#define AAX_PLUG_CATEGORY_STR "Synth"

#define VST3_SUBCATEGORY "Instrument|Synth"
