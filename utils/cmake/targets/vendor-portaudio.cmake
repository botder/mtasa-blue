#
# portaudio library target
#
if (NOT (MTASA_OS_WINDOWS AND MTASA_X86))
    return()
endif()

set(VENDOR_PORTAUDIO_DIR "${MTASA_VENDOR_DIR}/portaudio")

add_library(vendor-portaudio STATIC
    "${VENDOR_PORTAUDIO_DIR}/pa_allocation.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_allocation.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_asio.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_converters.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_converters.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_cpuload.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_cpuload.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_debugprint.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_debugprint.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_dither.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_dither.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_endianness.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_front.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_hostapi.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_jack.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_linux_alsa.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_mac_core.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_memorybarrier.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_process.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_process.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_ringbuffer.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_ringbuffer.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_stream.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_stream.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_trace.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_trace.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_types.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_util.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_ds.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_ds.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_ds_dynlink.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_ds_dynlink.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_hostapis.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_util.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_wasapi.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_waveformat.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_waveformat.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_wdmks_utils.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_wdmks_utils.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_win_wmme.h"
    "${VENDOR_PORTAUDIO_DIR}/pa_x86_plain_converters.c"
    "${VENDOR_PORTAUDIO_DIR}/pa_x86_plain_converters.h"
    "${VENDOR_PORTAUDIO_DIR}/portaudio.h"
)

target_include_directories(vendor-portaudio PUBLIC "${VENDOR_PORTAUDIO_DIR}")
