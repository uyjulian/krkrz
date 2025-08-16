cmake_minimum_required(VERSION 3.12)
project(krkrz)

if((NOT DEFINED CMAKE_BUILD_TYPE) OR (NOT CMAKE_BUILD_TYPE) OR (CMAKE_BUILD_TYPE STREQUAL ""))
set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

if((NOT DEFINED MY_LIBRARY_PROCESSOR) OR (NOT MY_LIBRARY_PROCESSOR) OR (MY_LIBRARY_PROCESSOR STREQUAL ""))
set(MY_LIBRARY_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
set(MY_LIBRARY_PROCESSOR intel32)
endif()
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64")
set(MY_LIBRARY_PROCESSOR intel64)
endif()
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
set(MY_LIBRARY_PROCESSOR arm32)
endif()
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
set(MY_LIBRARY_PROCESSOR arm64)
endif()
endif()

if((NOT DEFINED CUR_GIT_TAG) OR (NOT CUR_GIT_TAG) OR (CUR_GIT_TAG STREQUAL ""))
	set(CUR_GIT_TAG Unknown)
	find_package(Git)
	if(GIT_FOUND)
		if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
			execute_process(COMMAND ${GIT_EXECUTABLE} describe --abbrev=0 --tags
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE TMP_GIT_TAG
			RESULT_VARIABLE TMP_RES
			OUTPUT_STRIP_TRAILING_WHITESPACE)
			if(TMP_RES EQUAL "0")
				set(CUR_GIT_TAG ${TMP_GIT_TAG})
			endif()
		endif()
	endif()
endif()

list(APPEND BASE_SOURCES
	base/BinaryStream.cpp
	base/CharacterSet.cpp
	base/EventIntf.cpp
	base/PluginIntf.cpp
	base/ScriptMgnIntf.cpp
	base/StorageIntf.cpp
	base/SysInitIntf.cpp
	base/SystemIntf.cpp
	base/TextStream.cpp
	base/UtilStreams.cpp
	base/XP3Archive.cpp
	base/win32/EventImpl.cpp
	base/win32/FileSelector.cpp
	base/win32/FuncStubs.cpp
	base/win32/NativeEventQueue.cpp
	base/win32/PluginImpl.cpp
	base/win32/ScriptMgnImpl.cpp
	base/win32/StorageImpl.cpp
	base/win32/SusieArchive.cpp
	base/win32/SysInitImpl.cpp
	base/win32/SystemImpl.cpp
)

list(APPEND ENVIRON_SOURCES
	environ/TouchPoint.cpp
	environ/win32/Application.cpp
	environ/win32/CompatibleNativeFuncs.cpp
	environ/win32/ConfigFormUnit.cpp
	environ/win32/DetectCPU.cpp
	environ/win32/EmergencyExit.cpp
	environ/win32/MouseCursor.cpp
	environ/win32/SystemControl.cpp
	environ/win32/TVPWindow.cpp
	environ/win32/VersionFormUnit.cpp
	environ/win32/WindowFormUnit.cpp
	environ/win32/WindowsUtil.cpp
)

list(APPEND EXTENSION_SOURCES
	extension/Extension.cpp
)

list(APPEND MOVIE_SOURCES
	movie/win32/TVPVideoOverlay.cpp
)

list(APPEND MOVIE_SOURCES
	movie/win32/BufferRenderer.cpp
	movie/win32/CDemuxOutputPin.cpp
	movie/win32/CDemuxSource.cpp
	movie/win32/CMediaSeekingProxy.cpp
	movie/win32/CVMRCustomAllocatorPresenter9.cpp
	movie/win32/CWMAllocator.cpp
	movie/win32/CWMBuffer.cpp
	movie/win32/CWMReader.cpp
	movie/win32/DShowException.cpp
	movie/win32/IBufferRenderer_i.c
	movie/win32/IRendererBufferAccess_i.c
	movie/win32/IRendererBufferVideo_i.c
	movie/win32/MFByteStream.cpp
	movie/win32/MFPlayer.cpp
	movie/win32/PlayWindow.cpp
	movie/win32/init_guid_movie.c
	movie/win32/asyncio.cpp
	movie/win32/asyncrdr.cpp
	movie/win32/dslayerd.cpp
	movie/win32/dsmixer.cpp
	movie/win32/dsmovie.cpp
	movie/win32/dsoverlay.cpp
	movie/win32/krlmovie.cpp
	movie/win32/krmmovie.cpp
	movie/win32/krmovie.cpp
)

list(APPEND MSG_SOURCES
	${CMAKE_CURRENT_BINARY_DIR}/tvpwin32.rc
	msg/MsgIntf.cpp
	msg/win32/MsgImpl.cpp
	msg/win32/MsgLoad.cpp
	msg/win32/ReadOptionDesc.cpp
)

list(APPEND SOUND_SOURCES
	sound/MathAlgorithms.cpp
	sound/OpusCodecDecoder.cpp
	sound/PhaseVocoderDSP.cpp
	sound/PhaseVocoderFilter.cpp
	sound/QueueSoundBufferImpl.cpp
	sound/RealFFT.cpp
	sound/SoundBufferBaseImpl.cpp
	sound/SoundBufferBaseIntf.cpp
	sound/SoundDecodeThread.cpp
	sound/SoundEventThread.cpp
	sound/SoundPlayer.cpp
	sound/WaveFormatConverter.cpp
	sound/WaveIntf.cpp
	sound/WaveLoopManager.cpp
	sound/WaveSegmentQueue.cpp
	sound/win32/WaveImpl.cpp
	sound/win32/XAudio2Device.cpp
	sound/win32/tvpsnd.c
)

# list(APPEND SOUND_SSE2_SOURCES
# 	sound/MathAlgorithms_SSE.cpp
# 	sound/RealFFT_SSE.cpp
# 	sound/WaveFormatConverter_SSE.cpp
# 	sound/xmmlib.cpp
# )

list(APPEND TJS2_SOURCES
	tjs2/tjs.cpp
	tjs2/tjs.tab.cpp
	tjs2/tjsArray.cpp
	tjs2/tjsBinarySerializer.cpp
	tjs2/tjsByteCodeLoader.cpp
	tjs2/tjsCompileControl.cpp
	tjs2/tjsConfig.cpp
	tjs2/tjsConstArrayData.cpp
	tjs2/tjsDate.cpp
	tjs2/tjsDateParser.cpp
	tjs2/tjsDebug.cpp
	tjs2/tjsDictionary.cpp
	tjs2/tjsDisassemble.cpp
	tjs2/tjsError.cpp
	tjs2/tjsException.cpp
	tjs2/tjsGlobalStringMap.cpp
	tjs2/tjsInterCodeExec.cpp
	tjs2/tjsInterCodeGen.cpp
	tjs2/tjsInterface.cpp
	tjs2/tjsLex.cpp
	tjs2/tjsMT19937ar-cok.cpp
	tjs2/tjsMath.cpp
	tjs2/tjsMessage.cpp
	tjs2/tjsNamespace.cpp
	tjs2/tjsNative.cpp
	tjs2/tjsObject.cpp
	tjs2/tjsObjectExtendable.cpp
	tjs2/tjsOctPack.cpp
	tjs2/tjsRandomGenerator.cpp
	tjs2/tjsRegExp.cpp
	tjs2/tjsScriptBlock.cpp
	tjs2/tjsScriptCache.cpp
	tjs2/tjsSnprintf.cpp
	tjs2/tjsString.cpp
	tjs2/tjsUtils.cpp
	tjs2/tjsVariant.cpp
	tjs2/tjsVariantString.cpp
	tjs2/tjsdate.tab.cpp
	tjs2/tjspp.tab.cpp
)

list(APPEND UTILS_SOURCES
	utils/ClipboardIntf.cpp
	utils/DebugIntf.cpp
	utils/MiscUtility.cpp
	utils/Random.cpp
	utils/TVPTimer.cpp
	utils/ThreadIntf.cpp
	utils/TickCount.cpp
	utils/TimerIntf.cpp
	utils/TimerThread.cpp
	utils/VelocityTracker.cpp
	utils/cp932_uni.cpp
	utils/md5.c
	utils/uni_cp932.cpp
	utils/win32/ClipboardImpl.cpp
)

list(APPEND VISUAL_SOURCES
	visual/BitmapIntf.cpp
	visual/BitmapLayerTreeOwner.cpp
	visual/CharacterData.cpp
	visual/ComplexRect.cpp
	visual/DrawDevice.cpp
	visual/FontSystem.cpp
	visual/FreeType.cpp
	visual/FreeTypeFontRasterizer.cpp
	visual/GraphicsLoadThread.cpp
	visual/GraphicsLoaderIntf.cpp
	visual/IA32/detect_cpu.cpp
	visual/ImageFunction.cpp
	visual/LayerBitmapImpl.cpp
	visual/LayerBitmapIntf.cpp
	visual/LayerIntf.cpp
	visual/LayerManager.cpp
	visual/LayerTreeOwnerImpl.cpp
	visual/LoadJPEG.cpp
	visual/LoadJXR.cpp
	visual/LoadPNG.cpp
	visual/LoadTLG.cpp
	visual/NullDrawDevice.cpp
	visual/PrerenderedFont.cpp
	visual/RectItf.cpp
	visual/SaveTLG5.cpp
	visual/SaveTLG6.cpp
	visual/TransIntf.cpp
	visual/VideoOvlIntf.cpp
	visual/WindowIntf.cpp
	visual/gl/ResampleImage.cpp
	visual/gl/WeightFunctor.cpp
	visual/gl/blend_function.cpp
	visual/tvpgl.c
)

# list(APPEND VISUAL_SOURCES
# 	visual/IA32/tvpgl_ia32_intf.c
# )

list(APPEND VISUAL_OPENGL_SOURCES
	visual/opengl/CanvasIntf.cpp
	visual/opengl/DrawCycleTimer.cpp
	visual/opengl/GLFrameBufferObject.cpp
	visual/opengl/GLShaderUtil.cpp
	visual/opengl/Matrix32Intf.cpp
	visual/opengl/Matrix44Intf.cpp
	visual/opengl/OffscreenIntf.cpp
	visual/opengl/OpenGLPlatformWin32.cpp
	visual/opengl/OpenGLScreen.cpp
	visual/opengl/ShaderProgramIntf.cpp
	visual/opengl/TextureIntf.cpp
	visual/opengl/VertexBinderIntf.cpp
	visual/opengl/VertexBufferIntf.cpp
)

list(APPEND VISUAL_WIN32_SOURCES
	visual/win32/BasicDrawDevice.cpp
	visual/win32/BitmapBitsAlloc.cpp
	visual/win32/BitmapInfomation.cpp
	visual/win32/DInputMgn.cpp
	visual/win32/GDIFontRasterizer.cpp
	visual/win32/GraphicsLoaderImpl.cpp
	visual/win32/LayerImpl.cpp
	visual/win32/NativeFreeTypeFace.cpp
	visual/win32/TVPScreen.cpp
	visual/win32/TVPSysFont.cpp
	visual/win32/VSyncTimingThread.cpp
	visual/win32/VideoOvlImpl.cpp
	visual/win32/WindowImpl.cpp
)

# list(APPEND VISUAL_X86_SIMD_SOURCES
# 	visual/gl/ResampleImageAVX2.cpp
# 	visual/gl/ResampleImageSSE2.cpp
# 	visual/gl/adjust_color_sse2.cpp
# 	visual/gl/blend_function_avx2.cpp
# 	visual/gl/blend_function_sse2.cpp
# 	visual/gl/boxblur_sse2.cpp
# 	visual/gl/colorfill_sse2.cpp
# 	visual/gl/colormap_sse2.cpp
# 	visual/gl/pixelformat_sse2.cpp
# 	visual/gl/tlg_sse2.cpp
# 	visual/gl/univtrans_sse2.cpp
# 	visual/gl/x86simdutil.cpp
# 	visual/gl/x86simdutilAVX2.cpp
# )

list(APPEND LIBZ_SOURCES
	external/zlib/adler32.c
	external/zlib/compress.c
	external/zlib/crc32.c
	external/zlib/deflate.c
	external/zlib/gzclose.c
	external/zlib/gzlib.c
	external/zlib/gzread.c
	external/zlib/gzwrite.c
	external/zlib/infback.c
	external/zlib/inffast.c
	external/zlib/inflate.c
	external/zlib/inftrees.c
	external/zlib/trees.c
	external/zlib/uncompr.c
	external/zlib/zutil.c
)

list(APPEND LIBPNG_SOURCES
	external/lpng/png.c
	external/lpng/pngerror.c
	external/lpng/pngget.c
	external/lpng/pngmem.c
	external/lpng/pngpread.c
	external/lpng/pngread.c
	external/lpng/pngrio.c
	external/lpng/pngrtran.c
	external/lpng/pngrutil.c
	external/lpng/pngset.c
	external/lpng/pngtrans.c
	external/lpng/pngwio.c
	external/lpng/pngwrite.c
	external/lpng/pngwtran.c
	external/lpng/pngwutil.c
)

list(APPEND LIBONIG_SOURCES
	external/onig/src/regerror.c
	external/onig/src/regparse.c
	external/onig/src/regext.c
	external/onig/src/regcomp.c
	external/onig/src/regexec.c
	external/onig/src/reggnu.c
	external/onig/src/regenc.c
	external/onig/src/regsyntax.c
	external/onig/src/regtrav.c
	external/onig/src/regversion.c
	external/onig/src/st.c
	external/onig/src/onig_init.c
	external/onig/src/unicode.c
	external/onig/src/ascii.c
	external/onig/src/utf8.c
	external/onig/src/utf16_be.c
	external/onig/src/utf16_le.c
	external/onig/src/utf32_be.c
	external/onig/src/utf32_le.c
	external/onig/src/euc_jp.c
	external/onig/src/sjis.c
	external/onig/src/iso8859_1.c
	external/onig/src/iso8859_2.c
	external/onig/src/iso8859_3.c
	external/onig/src/iso8859_4.c
	external/onig/src/iso8859_5.c
	external/onig/src/iso8859_6.c
	external/onig/src/iso8859_7.c
	external/onig/src/iso8859_8.c
	external/onig/src/iso8859_9.c
	external/onig/src/iso8859_10.c
	external/onig/src/iso8859_11.c
	external/onig/src/iso8859_13.c
	external/onig/src/iso8859_14.c
	external/onig/src/iso8859_15.c
	external/onig/src/iso8859_16.c
	external/onig/src/euc_tw.c
	external/onig/src/euc_kr.c
	external/onig/src/big5.c
	external/onig/src/gb18030.c
	external/onig/src/koi8_r.c
	external/onig/src/cp1251.c
	external/onig/src/euc_jp_prop.c
	external/onig/src/sjis_prop.c
	external/onig/src/unicode_unfold_key.c
	external/onig/src/unicode_fold1_key.c
	external/onig/src/unicode_fold2_key.c
	external/onig/src/unicode_fold3_key.c
)

list(APPEND LIBJPEG_SOURCES
	external/libjpeg-turbo/jaricom.c
	external/libjpeg-turbo/jcapimin.c
	external/libjpeg-turbo/jcapistd.c
	external/libjpeg-turbo/jcarith.c
	external/libjpeg-turbo/jccoefct.c
	external/libjpeg-turbo/jccolor.c
	external/libjpeg-turbo/jcdctmgr.c
	external/libjpeg-turbo/jchuff.c
	external/libjpeg-turbo/jcinit.c
	external/libjpeg-turbo/jcmainct.c
	external/libjpeg-turbo/jcmarker.c
	external/libjpeg-turbo/jcmaster.c
	external/libjpeg-turbo/jcomapi.c
	external/libjpeg-turbo/jcparam.c
	external/libjpeg-turbo/jcphuff.c
	external/libjpeg-turbo/jcprepct.c
	external/libjpeg-turbo/jcsample.c
	external/libjpeg-turbo/jctrans.c
	external/libjpeg-turbo/jdapimin.c
	external/libjpeg-turbo/jdapistd.c
	external/libjpeg-turbo/jdarith.c
	external/libjpeg-turbo/jdatadst-tj.c
	external/libjpeg-turbo/jdatadst.c
	external/libjpeg-turbo/jdatasrc-tj.c
	external/libjpeg-turbo/jdatasrc.c
	external/libjpeg-turbo/jdcoefct.c
	external/libjpeg-turbo/jdcolor.c
	external/libjpeg-turbo/jddctmgr.c
	external/libjpeg-turbo/jdhuff.c
	external/libjpeg-turbo/jdinput.c
	external/libjpeg-turbo/jdmainct.c
	external/libjpeg-turbo/jdmarker.c
	external/libjpeg-turbo/jdmaster.c
	external/libjpeg-turbo/jdmerge.c
	external/libjpeg-turbo/jdphuff.c
	external/libjpeg-turbo/jdpostct.c
	external/libjpeg-turbo/jdsample.c
	external/libjpeg-turbo/jdtrans.c
	external/libjpeg-turbo/jerror.c
	external/libjpeg-turbo/jfdctflt.c
	external/libjpeg-turbo/jfdctfst.c
	external/libjpeg-turbo/jfdctint.c
	external/libjpeg-turbo/jidctflt.c
	external/libjpeg-turbo/jidctfst.c
	external/libjpeg-turbo/jidctint.c
	external/libjpeg-turbo/jidctred.c
	external/libjpeg-turbo/jmemmgr.c
	external/libjpeg-turbo/jmemnobs.c
	external/libjpeg-turbo/jquant1.c
	external/libjpeg-turbo/jquant2.c
	external/libjpeg-turbo/jutils.c
	external/libjpeg-turbo/transupp.c
	external/libjpeg-turbo/turbojpeg.c
)

list(APPEND LIBJPEG_SIMD_SOURCES
	external/libjpeg-turbo/jsimd_none.c
)

# list(APPEND LIBJPEG_SIMD_SOURCES
# 	external/libjpeg-turbo/simd/jccolor-mmx.asm
# 	external/libjpeg-turbo/simd/jccolor-sse2.asm
# 	external/libjpeg-turbo/simd/jcgray-mmx.asm
# 	external/libjpeg-turbo/simd/jcgray-sse2.asm
# 	external/libjpeg-turbo/simd/jchuff-sse2.asm
# 	external/libjpeg-turbo/simd/jcsample-mmx.asm
# 	external/libjpeg-turbo/simd/jcsample-sse2.asm
# 	external/libjpeg-turbo/simd/jdcolor-mmx.asm
# 	external/libjpeg-turbo/simd/jdcolor-sse2.asm
# 	external/libjpeg-turbo/simd/jdmerge-mmx.asm
# 	external/libjpeg-turbo/simd/jdmerge-sse2.asm
# 	external/libjpeg-turbo/simd/jdsample-mmx.asm
# 	external/libjpeg-turbo/simd/jdsample-sse2.asm
# 	external/libjpeg-turbo/simd/jfdctflt-3dn.asm
# 	external/libjpeg-turbo/simd/jfdctflt-sse.asm
# 	external/libjpeg-turbo/simd/jfdctfst-mmx.asm
# 	external/libjpeg-turbo/simd/jfdctfst-sse2.asm
# 	external/libjpeg-turbo/simd/jfdctint-mmx.asm
# 	external/libjpeg-turbo/simd/jfdctint-sse2.asm
# 	external/libjpeg-turbo/simd/jidctflt-3dn.asm
# 	external/libjpeg-turbo/simd/jidctflt-sse.asm
# 	external/libjpeg-turbo/simd/jidctflt-sse2.asm
# 	external/libjpeg-turbo/simd/jidctfst-mmx.asm
# 	external/libjpeg-turbo/simd/jidctfst-sse2.asm
# 	external/libjpeg-turbo/simd/jidctint-mmx.asm
# 	external/libjpeg-turbo/simd/jidctint-sse2.asm
# 	external/libjpeg-turbo/simd/jidctred-mmx.asm
# 	external/libjpeg-turbo/simd/jidctred-sse2.asm
# 	external/libjpeg-turbo/simd/jquant-3dn.asm
# 	external/libjpeg-turbo/simd/jquant-mmx.asm
# 	external/libjpeg-turbo/simd/jquant-sse.asm
# 	external/libjpeg-turbo/simd/jquantf-sse2.asm
# 	external/libjpeg-turbo/simd/jquanti-sse2.asm
# 	external/libjpeg-turbo/simd/jsimdcpu.asm
# )

list(APPEND LIBFREETYPE_SOURCES
	external/freetype/src/autofit/autofit.c
	external/freetype/src/bdf/bdf.c
	external/freetype/src/cff/cff.c
	external/freetype/src/base/ftbase.c
	external/freetype/src/base/ftbitmap.c
	external/freetype/src/cache/ftcache.c
	external/freetype/builds/windows/ftdebug.c
	external/freetype/src/base/ftfstype.c
	external/freetype/src/base/ftgasp.c
	external/freetype/src/base/ftglyph.c
	external/freetype/src/gzip/ftgzip.c
	external/freetype/src/base/ftinit.c
	external/freetype/src/lzw/ftlzw.c
	external/freetype/src/base/ftstroke.c
	external/freetype/src/base/ftsystem.c
	external/freetype/src/smooth/smooth.c
	external/freetype/src/base/ftbbox.c
	external/freetype/src/base/ftfntfmt.c
	external/freetype/src/base/ftmm.c
	external/freetype/src/base/ftpfr.c
	external/freetype/src/base/ftsynth.c
	external/freetype/src/base/fttype1.c
	external/freetype/src/base/ftwinfnt.c
	external/freetype/src/base/ftlcdfil.c
	external/freetype/src/base/ftgxval.c
	external/freetype/src/base/ftotval.c
	external/freetype/src/base/ftpatent.c
	external/freetype/src/pcf/pcf.c
	external/freetype/src/pfr/pfr.c
	external/freetype/src/psaux/psaux.c
	external/freetype/src/pshinter/pshinter.c
	external/freetype/src/psnames/psmodule.c
	external/freetype/src/raster/raster.c
	external/freetype/src/sfnt/sfnt.c
	external/freetype/src/truetype/truetype.c
	external/freetype/src/type1/type1.c
	external/freetype/src/cid/type1cid.c
	external/freetype/src/type42/type42.c
	external/freetype/src/winfonts/winfnt.c
)

list(APPEND LIBOGG_SOURCES
	external/libogg/src/bitwise.c
	external/libogg/src/framing.c
)

list(APPEND LIBOPUS_SOURCES
	external/opus/celt/bands.c
	external/opus/celt/celt.c
	external/opus/celt/celt_decoder.c
	external/opus/celt/celt_encoder.c
	external/opus/celt/celt_lpc.c
	external/opus/celt/cwrs.c
	external/opus/celt/entcode.c
	external/opus/celt/entdec.c
	external/opus/celt/entenc.c
	external/opus/celt/kiss_fft.c
	external/opus/celt/laplace.c
	external/opus/celt/mathops.c
	external/opus/celt/mdct.c
	external/opus/celt/modes.c
	external/opus/celt/pitch.c
	external/opus/celt/quant_bands.c
	external/opus/celt/rate.c
	external/opus/celt/vq.c
	external/opus/src/analysis.c
	external/opus/src/mlp.c
	external/opus/src/mlp_data.c
	external/opus/src/opus.c
	external/opus/src/opus_compare.c
	external/opus/src/opus_decoder.c
	external/opus/src/opus_encoder.c
	external/opus/src/opus_multistream.c
	external/opus/src/opus_multistream_decoder.c
	external/opus/src/opus_multistream_encoder.c
	external/opus/src/repacketizer.c
	external/opus/silk/A2NLSF.c
	external/opus/silk/ana_filt_bank_1.c
	external/opus/silk/biquad_alt.c
	external/opus/silk/bwexpander.c
	external/opus/silk/bwexpander_32.c
	external/opus/silk/check_control_input.c
	external/opus/silk/CNG.c
	external/opus/silk/code_signs.c
	external/opus/silk/control_audio_bandwidth.c
	external/opus/silk/control_codec.c
	external/opus/silk/control_SNR.c
	external/opus/silk/debug.c
	external/opus/silk/decoder_set_fs.c
	external/opus/silk/decode_core.c
	external/opus/silk/decode_frame.c
	external/opus/silk/decode_indices.c
	external/opus/silk/decode_parameters.c
	external/opus/silk/decode_pitch.c
	external/opus/silk/decode_pulses.c
	external/opus/silk/dec_API.c
	external/opus/silk/encode_indices.c
	external/opus/silk/encode_pulses.c
	external/opus/silk/enc_API.c
	external/opus/silk/gain_quant.c
	external/opus/silk/HP_variable_cutoff.c
	external/opus/silk/init_decoder.c
	external/opus/silk/init_encoder.c
	external/opus/silk/inner_prod_aligned.c
	external/opus/silk/interpolate.c
	external/opus/silk/lin2log.c
	external/opus/silk/log2lin.c
	external/opus/silk/LPC_analysis_filter.c
	external/opus/silk/LPC_inv_pred_gain.c
	external/opus/silk/LP_variable_cutoff.c
	external/opus/silk/NLSF2A.c
	external/opus/silk/NLSF_decode.c
	external/opus/silk/NLSF_del_dec_quant.c
	external/opus/silk/NLSF_encode.c
	external/opus/silk/NLSF_stabilize.c
	external/opus/silk/NLSF_unpack.c
	external/opus/silk/NLSF_VQ.c
	external/opus/silk/NLSF_VQ_weights_laroia.c
	external/opus/silk/NSQ.c
	external/opus/silk/NSQ_del_dec.c
	external/opus/silk/pitch_est_tables.c
	external/opus/silk/PLC.c
	external/opus/silk/process_NLSFs.c
	external/opus/silk/quant_LTP_gains.c
	external/opus/silk/resampler.c
	external/opus/silk/resampler_down2.c
	external/opus/silk/resampler_down2_3.c
	external/opus/silk/resampler_private_AR2.c
	external/opus/silk/resampler_private_down_FIR.c
	external/opus/silk/resampler_private_IIR_FIR.c
	external/opus/silk/resampler_private_up2_HQ.c
	external/opus/silk/resampler_rom.c
	external/opus/silk/shell_coder.c
	external/opus/silk/sigm_Q15.c
	external/opus/silk/sort.c
	external/opus/silk/stereo_decode_pred.c
	external/opus/silk/stereo_encode_pred.c
	external/opus/silk/stereo_find_predictor.c
	external/opus/silk/stereo_LR_to_MS.c
	external/opus/silk/stereo_MS_to_LR.c
	external/opus/silk/stereo_quant_pred.c
	external/opus/silk/sum_sqr_shift.c
	external/opus/silk/tables_gain.c
	external/opus/silk/tables_LTP.c
	external/opus/silk/tables_NLSF_CB_NB_MB.c
	external/opus/silk/tables_NLSF_CB_WB.c
	external/opus/silk/tables_other.c
	external/opus/silk/tables_pitch_lag.c
	external/opus/silk/tables_pulses_per_block.c
	external/opus/silk/table_LSF_cos.c
	external/opus/silk/VAD.c
	external/opus/silk/VQ_WMat_EC.c
	external/opus/silk/float/apply_sine_window_FLP.c
	external/opus/silk/float/autocorrelation_FLP.c
	external/opus/silk/float/burg_modified_FLP.c
	external/opus/silk/float/bwexpander_FLP.c
	external/opus/silk/float/corrMatrix_FLP.c
	external/opus/silk/float/encode_frame_FLP.c
	external/opus/silk/float/energy_FLP.c
	external/opus/silk/float/find_LPC_FLP.c
	external/opus/silk/float/find_LTP_FLP.c
	external/opus/silk/float/find_pitch_lags_FLP.c
	external/opus/silk/float/find_pred_coefs_FLP.c
	external/opus/silk/float/inner_product_FLP.c
	external/opus/silk/float/k2a_FLP.c
	external/opus/silk/float/levinsondurbin_FLP.c
	external/opus/silk/float/LPC_analysis_filter_FLP.c
	external/opus/silk/float/LPC_inv_pred_gain_FLP.c
	external/opus/silk/float/LTP_analysis_filter_FLP.c
	external/opus/silk/float/LTP_scale_ctrl_FLP.c
	external/opus/silk/float/noise_shape_analysis_FLP.c
	external/opus/silk/float/pitch_analysis_core_FLP.c
	external/opus/silk/float/prefilter_FLP.c
	external/opus/silk/float/process_gains_FLP.c
	external/opus/silk/float/regularize_correlations_FLP.c
	external/opus/silk/float/residual_energy_FLP.c
	external/opus/silk/float/scale_copy_vector_FLP.c
	external/opus/silk/float/scale_vector_FLP.c
	external/opus/silk/float/schur_FLP.c
	external/opus/silk/float/solve_LS_FLP.c
	external/opus/silk/float/sort_FLP.c
	external/opus/silk/float/warped_autocorrelation_FLP.c
	external/opus/silk/float/wrappers_FLP.c
)

# list(APPEND LIBOPUS_SOURCES
# 	external/opus/silk/fixed/apply_sine_window_FIX.c
# 	external/opus/silk/fixed/autocorr_FIX.c
# 	external/opus/silk/fixed/burg_modified_FIX.c
# 	external/opus/silk/fixed/corrMatrix_FIX.c
# 	external/opus/silk/fixed/encode_frame_FIX.c
# 	external/opus/silk/fixed/find_LPC_FIX.c
# 	external/opus/silk/fixed/find_LTP_FIX.c
# 	external/opus/silk/fixed/find_pitch_lags_FIX.c
# 	external/opus/silk/fixed/find_pred_coefs_FIX.c
# 	external/opus/silk/fixed/k2a_FIX.c
# 	external/opus/silk/fixed/k2a_Q16_FIX.c
# 	external/opus/silk/fixed/LTP_analysis_filter_FIX.c
# 	external/opus/silk/fixed/LTP_scale_ctrl_FIX.c
# 	external/opus/silk/fixed/noise_shape_analysis_FIX.c
# 	external/opus/silk/fixed/pitch_analysis_core_FIX.c
# 	external/opus/silk/fixed/prefilter_FIX.c
# 	external/opus/silk/fixed/process_gains_FIX.c
# 	external/opus/silk/fixed/regularize_correlations_FIX.c
# 	external/opus/silk/fixed/residual_energy16_FIX.c
# 	external/opus/silk/fixed/residual_energy_FIX.c
# 	external/opus/silk/fixed/schur64_FIX.c
# 	external/opus/silk/fixed/schur_FIX.c
# 	external/opus/silk/fixed/solve_LS_FIX.c
# 	external/opus/silk/fixed/vector_ops_FIX.c
# 	external/opus/silk/fixed/warped_autocorrelation_FIX.c
# )

list(APPEND LIBOPUSFILE_SOURCES
	external/opusfile/src/http.c
	external/opusfile/src/info.c
	external/opusfile/src/internal.c
	external/opusfile/src/opusfile.c
	external/opusfile/src/stream.c
	external/opusfile/src/wincerts.c
)

list(APPEND BASECLASSES_SOURCES
	external/baseclasses/amextra.cpp
	external/baseclasses/amfilter.cpp
	external/baseclasses/amvideo.cpp
	external/baseclasses/arithutil.cpp
	external/baseclasses/combase.cpp
	external/baseclasses/cprop.cpp
	external/baseclasses/ctlutil.cpp
	external/baseclasses/ddmm.cpp
	external/baseclasses/dllentry.cpp
	external/baseclasses/dllsetup.cpp
	external/baseclasses/mtype.cpp
	external/baseclasses/outputq.cpp
	external/baseclasses/perflog.cpp
	external/baseclasses/pstream.cpp
	external/baseclasses/pullpin.cpp
	external/baseclasses/refclock.cpp
	external/baseclasses/renbase.cpp
	external/baseclasses/schedule.cpp
	external/baseclasses/seekpt.cpp
	external/baseclasses/source.cpp
	external/baseclasses/strmctl.cpp
	external/baseclasses/sysclock.cpp
	external/baseclasses/transfrm.cpp
	external/baseclasses/transip.cpp
	external/baseclasses/videoctl.cpp
	external/baseclasses/vtrans.cpp
	external/baseclasses/winctrl.cpp
	external/baseclasses/winutil.cpp
	external/baseclasses/wxdebug.cpp
	external/baseclasses/wxlist.cpp
	external/baseclasses/wxutil.cpp
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/string_table_chs.rc
	COMMAND iconv -f UTF-16 -t UTF8 ${CMAKE_CURRENT_SOURCE_DIR}/vcproj/string_table_chs.rc > ${CMAKE_CURRENT_BINARY_DIR}/string_table_chs.rc
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/vcproj/string_table_chs.rc
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/string_table_en.rc
	COMMAND iconv -f UTF-16 -t UTF8 ${CMAKE_CURRENT_SOURCE_DIR}/vcproj/string_table_en.rc > ${CMAKE_CURRENT_BINARY_DIR}/string_table_en.rc
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/vcproj/string_table_en.rc
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/string_table_jp.rc
	COMMAND iconv -f UTF-16 -t UTF8 ${CMAKE_CURRENT_SOURCE_DIR}/vcproj/string_table_jp.rc > ${CMAKE_CURRENT_BINARY_DIR}/string_table_jp.rc
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/vcproj/string_table_jp.rc
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tvpwin32.rc
	COMMAND iconv -f UTF-16 -t UTF8 ${CMAKE_CURRENT_SOURCE_DIR}/vcproj/tvpwin32.rc > ${CMAKE_CURRENT_BINARY_DIR}/tvpwin32.rc
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/vcproj/tvpwin32.rc ${CMAKE_CURRENT_BINARY_DIR}/string_table_chs.rc ${CMAKE_CURRENT_BINARY_DIR}/string_table_en.rc ${CMAKE_CURRENT_BINARY_DIR}/string_table_jp.rc
)

SET(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -c65001")

set(MY_LIBRARY_NAME ${PROJECT_NAME})
list(APPEND MY_LIBRARY_DEFINITIONS
	UNICODE
	_UNICODE
	NDEBUG
	_NDEBUG
	WIN32
	_WIN32
	_WINDOWS
	NO_STRICT
	HAVE_CONFIG_H
	FT2_BUILD_LIBRARY
	USE_ALLOCA
	OPUS_BUILD
	HAVE_LRINTF
	HAVE_LRINT
	FLOAT_APPROX
	MINGW_HAS_SECURE_API
	PNG_ARM_NEON_OPT=0
	TVP_LOG_TO_COMMANDLINE_CONSOLE
	TJS_TEXT_OUT_CRLF
	TJS_JP_LOCALIZED
	TJS_DEBUG_DUMP_STRING
	TVP_OPUS_DECODER_IMPLEMENT
	TVP_REPORT_HW_EXCEPTION
)
list(APPEND MY_LIBRARY_CFLAGS
	-Wall
	-Wno-unused-value
	-Wno-unused-variable
	-Wno-format
	-flto
	-fno-delete-null-pointer-checks
	-fno-strict-aliasing
)
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
list(APPEND MY_LIBRARY_CFLAGS
	-march=pentium4
	-mfpmath=sse
)
endif()
list(APPEND MY_LIBRARY_CXXFLAGS
	${MY_LIBRARY_CFLAGS}
	-Wno-reorder
	-fpermissive
)
list(APPEND MY_LIBRARY_LDFLAGS
	-static
	-static-libstdc++
	-static-libgcc
	-municode
	-fPIC
	-flto
)
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
list(APPEND MY_LIBRARY_LDFLAGS
	-march=pentium4
	-mfpmath=sse
)
endif()
list(APPEND MY_LIBRARY_LIBS
	-lwinmm
	-lws2_32
	-lcomctl32
	-lgdi32
	-lwinhttp
	-lpsapi
	-luser32
	-lcomdlg32
	-lole32
	-lshell32
	-ladvapi32
	-loleaut32
	-limm32
	-lversion
	-lshlwapi
	-ldbghelp
	-luuid
	-lmpr
	-ldmoguids
	-lmfplat
	-lstrmiids
	-lmf
	-lpropsys
	-lmfuuid
	-lamstrmid
	-ldxguid
	-lwindowscodecs
)
if((${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686") OR (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64"))
list(APPEND MY_LIBRARY_LIBS
	-lquartz
)
endif()
list(APPEND MY_LIBRARY_SRC
	${BASE_SOURCES}
	${ENVIRON_SOURCES}
	${EXTENSION_SOURCES}
	${MOVIE_SOURCES}
	${MSG_SOURCES}
	${SOUND_SOURCES}
	${SOUND_SSE2_SOURCES}
	${TJS2_SOURCES}
	${UTILS_SOURCES}
	${VISUAL_SOURCES}
	${VISUAL_OPENGL_SOURCES}
	${VISUAL_WIN32_SOURCES}
	${VISUAL_X86_SIMD_SOURCES}
	${LIBZ_SOURCES}
	${LIBPNG_SOURCES}
	${LIBONIG_SOURCES}
	${LIBJPEG_SOURCES}
	${LIBJPEG_SIMD_SOURCES}
	${LIBFREETYPE_SOURCES}
	${BASECLASSES_SOURCES}
	${LIBOGG_SOURCES}
	${LIBOPUS_SOURCES}
	${LIBOPUSFILE_SOURCES}
	${BASECLASSES_SOURCES}
)
list(APPEND MY_LIBRARY_INC
	${CMAKE_CURRENT_SOURCE_DIR}
	base
	base/win32
	environ
	environ/win32
	extension
	external
	external/angle/include
	external/baseclasses
	external/freetype/include
	external/freetype/src
	external/freetype/devel
	external/glm
	external/jxrlib/image/sys
	external/jxrlib/jxrgluelib
	external/libjpeg-turbo
	external/libjpeg-turbo/vcproj
	external/libjpeg-turbo/win
	external/libogg/include
	external/lpng
	external/onig
	external/onig/src
	external/libogg/include
	external/opus/celt
	external/opus/include
	external/opus/win32
	external/opusfile/include
	external/opusfile/src
	external/zlib
	movie/win32
	msg
	msg/win32
	platform/win32
	sound
	sound/win32
	tjs2
	utils
	utils/win32
	vcproj
	visual
	visual/IA32
	visual/gl
	visual/opengl
	visual/win32
)

set_source_files_properties(visual/LoadPNG.cpp PROPERTIES COMPILE_FLAGS -O1)
set_source_files_properties(${LIBOPUS_SOURCES} PROPERTIES INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/external/opus/silk;${CMAKE_CURRENT_SOURCE_DIR}/external/opus/silk/fixed;${CMAKE_CURRENT_SOURCE_DIR}/external/opus/silk/float")

add_executable(${MY_LIBRARY_NAME})
if(NOT (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686"))
set_target_properties(${MY_LIBRARY_NAME} PROPERTIES OUTPUT_NAME tvpwin32_${MY_LIBRARY_PROCESSOR})
else()
set_target_properties(${MY_LIBRARY_NAME} PROPERTIES OUTPUT_NAME tvpwin32)
endif()
set_target_properties(${MY_LIBRARY_NAME} PROPERTIES PREFIX "")
set_target_properties(${MY_LIBRARY_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set_target_properties(${MY_LIBRARY_NAME} PROPERTIES C_STANDARD 11)
set_target_properties(${MY_LIBRARY_NAME} PROPERTIES CXX_STANDARD 14)
target_sources(${MY_LIBRARY_NAME} PRIVATE ${MY_LIBRARY_SRC})
target_compile_options(${MY_LIBRARY_NAME} PUBLIC $<$<COMPILE_LANGUAGE:C>:${MY_LIBRARY_CFLAGS}>)
target_compile_options(${MY_LIBRARY_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${MY_LIBRARY_CXXFLAGS}>)
target_compile_definitions(${MY_LIBRARY_NAME} PUBLIC ${MY_LIBRARY_DEFINITIONS})
target_include_directories(${MY_LIBRARY_NAME} PUBLIC ${MY_LIBRARY_INC})
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13.0")
target_link_options(${MY_LIBRARY_NAME} PUBLIC ${MY_LIBRARY_LDFLAGS})
endif()
target_link_libraries(${MY_LIBRARY_NAME} ${MY_LIBRARY_LIBS})

if((NOT DEFINED MY_ARCHIVE_NAME) OR (NOT MY_ARCHIVE_NAME) OR (MY_ARCHIVE_NAME STREQUAL ""))
if((NOT DEFINED USE_ARCHIVE_HAS_GIT_TAG) OR (NOT USE_ARCHIVE_HAS_GIT_TAG) OR (USE_ARCHIVE_HAS_GIT_TAG STREQUAL ""))
set(USE_ARCHIVE_HAS_GIT_TAG FALSE)
endif()
if(USE_ARCHIVE_HAS_GIT_TAG)
set(MY_ARCHIVE_NAME ${MY_LIBRARY_NAME}.${MY_LIBRARY_PROCESSOR}.${CUR_GIT_TAG}.7z)
else()
set(MY_ARCHIVE_NAME ${MY_LIBRARY_NAME}.${MY_LIBRARY_PROCESSOR}.7z)
endif()
endif()
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MY_ARCHIVE_NAME}
	COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/${MY_ARCHIVE_NAME} && 7z a ${CMAKE_CURRENT_BINARY_DIR}/${MY_ARCHIVE_NAME} $<TARGET_FILE:${MY_LIBRARY_NAME}>
	DEPENDS ${MY_LIBRARY_NAME}
)
add_custom_target(archive
	DEPENDS ${MY_ARCHIVE_NAME}
)

if(FALSE)
cmake_minimum_required(VERSION 3.10.2)

if( ANDROID )
	set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
	set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -DNDEBUG")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -s -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g3 -Og")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -s -DNDEBUG")
	set(CMAKE_C_FLAGS_MINSIZEREL "-Os -s -DNDEBUG")
endif()


set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNICODE -DTJS_TEXT_OUT_CRLF -DTJS_JP_LOCALIZED -DTVP_ENABLE_EXECUTE_AT_EXCEPTION -DTVP_LOG_TO_COMMANDLINE_CONSOLE" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNICODE -DTJS_TEXT_OUT_CRLF -DTJS_JP_LOCALIZED -DTVP_ENABLE_EXECUTE_AT_EXCEPTION -DTVP_LOG_TO_COMMANDLINE_CONSOLE" )


# 共通の include ディレクトリ
set( KRKRZ_INC
environ/
tjs2
external/onig/src
external/lpng
external/freetype/include
external/libjpeg-turbo
external/libjpeg-turbo/android
external/libogg/include
external/opus/include
external/opusfile/include
external/glm/
base
extension
sound
movie
msg
utils
visual
visual/gl
visual/opengl )

# Android 用の include ディレクトリ
if( ANDROID )
	list(APPEND KRKRZ_INC 
	environ/android
	base/android
	sound/android
	msg/android
	utils/android
	visual/android
	)
elseif( WIN32 )
	list(APPEND KRKRZ_INC 
	environ/win32
	base/win32
	sound/win32
	msg/win32
	utils/win32
	visual/win32
	)
endif()

include_directories( ${KRKRZ_INC} )

set( KRKRZ_SRC 
tjs2/tjs.cpp
tjs2/tjs.tab.cpp
tjs2/tjsArray.cpp
tjs2/tjsBinarySerializer.cpp
tjs2/tjsByteCodeLoader.cpp
tjs2/tjsCompileControl.cpp
tjs2/tjsConfig.cpp
tjs2/tjsConstArrayData.cpp
tjs2/tjsDate.cpp
tjs2/tjsdate.tab.cpp
tjs2/tjsDateParser.cpp
tjs2/tjsDebug.cpp
tjs2/tjsDictionary.cpp
tjs2/tjsDisassemble.cpp
tjs2/tjsError.cpp
tjs2/tjsException.cpp
tjs2/tjsGlobalStringMap.cpp
tjs2/tjsInterCodeExec.cpp
tjs2/tjsInterCodeGen.cpp
tjs2/tjsInterface.cpp
tjs2/tjsLex.cpp
tjs2/tjsMath.cpp
tjs2/tjsMessage.cpp
tjs2/tjsMT19937ar-cok.cpp
tjs2/tjsNamespace.cpp
tjs2/tjsNative.cpp
tjs2/tjsObject.cpp
tjs2/tjsObjectExtendable.cpp
tjs2/tjsOctPack.cpp
tjs2/tjspp.tab.cpp
tjs2/tjsRandomGenerator.cpp
tjs2/tjsRegExp.cpp
tjs2/tjsScriptBlock.cpp
tjs2/tjsScriptCache.cpp
tjs2/tjsSnprintf.cpp
tjs2/tjsString.cpp
tjs2/tjsUtils.cpp
tjs2/tjsVariant.cpp
tjs2/tjsVariantString.cpp
base/BinaryStream.cpp
base/CharacterSet.cpp
base/EventIntf.cpp
base/PluginIntf.cpp
base/ScriptMgnIntf.cpp
base/StorageIntf.cpp
base/SysInitIntf.cpp
base/SystemIntf.cpp
base/TextStream.cpp
base/UtilStreams.cpp
base/XP3Archive.cpp
base/android/AssetMedia.cpp
base/android/ContentMedia.cpp
base/android/EventImpl.cpp
base/android/FuncStubs.cpp
base/android/SystemImpl.cpp
base/android/NativeEventQueue.cpp
base/android/PluginImpl.cpp
base/android/ScriptMgnImpl.cpp
base/android/StorageImpl.cpp
base/android/SysInitImpl.cpp
environ/TouchPoint.cpp
environ/android/Application.cpp
environ/android/DetectCPU.cpp
environ/android/TVPScreen.cpp
environ/android/SystemControl.cpp
environ/android/VirtualKey.cpp
environ/android/WindowForm.cpp
extension/Extension.cpp
msg/MsgIntf.cpp
msg/android/MsgImpl.cpp
msg/android/MsgLoad.cpp
sound/MathAlgorithms.cpp
sound/PhaseVocoderDSP.cpp
sound/PhaseVocoderFilter.cpp
sound/QueueSoundBufferImpl.cpp
sound/RealFFT.cpp
sound/SoundBufferBaseIntf.cpp
sound/SoundBufferBaseImpl.cpp
sound/SoundDecodeThread.cpp
sound/SoundEventThread.cpp
sound/SoundPlayer.cpp
sound/WaveFormatConverter.cpp
sound/WaveIntf.cpp
sound/WaveLoopManager.cpp
sound/WaveSegmentQueue.cpp
sound/OpusCodecDecoder.cpp
sound/android/OpenSLESAudioDevice.cpp
utils/ClipboardIntf.cpp
utils/cp932_uni.cpp
utils/DebugIntf.cpp
utils/md5.c
utils/MiscUtility.cpp
utils/Random.cpp
utils/ThreadIntf.cpp
utils/TickCount.cpp
utils/TimerThread.cpp
utils/TimerIntf.cpp
utils/TVPTimer.cpp
utils/uni_cp932.cpp
utils/VelocityTracker.cpp
utils/android/ClipboardImpl.cpp
visual/BitmapIntf.cpp
visual/BitmapLayerTreeOwner.cpp
visual/CharacterData.cpp
visual/ComplexRect.cpp
visual/DrawDevice.cpp
visual/FontSystem.cpp
visual/FreeType.cpp
visual/FreeTypeFontRasterizer.cpp
visual/GraphicsLoaderIntf.cpp
visual/GraphicsLoadThread.cpp
visual/ImageFunction.cpp
visual/LayerBitmapImpl.cpp
visual/LayerBitmapIntf.cpp
visual/LayerIntf.cpp
visual/LayerManager.cpp
visual/LayerTreeOwnerImpl.cpp
visual/LoadJPEG.cpp
visual/LoadPNG.cpp
visual/LoadTLG.cpp
visual/NullDrawDevice.cpp
visual/PrerenderedFont.cpp
visual/RectItf.cpp
visual/SaveTLG5.cpp
visual/SaveTLG6.cpp
visual/TransIntf.cpp
visual/tvpgl.c
visual/VideoOvlIntf.cpp
visual/WindowIntf.cpp
visual/android/BitmapBitsAlloc.cpp
visual/android/BitmapInfomation.cpp
visual/android/LayerImpl.cpp
visual/android/VideoOvlImpl.cpp
visual/android/WindowImpl.cpp
visual/gl/blend_function.cpp
visual/gl/ResampleImage.cpp
visual/gl/WeightFunctor.cpp
visual/opengl/CanvasIntf.cpp
visual/opengl/DrawCycleTimer.cpp
visual/opengl/GLFrameBufferObject.cpp
visual/opengl/GLShaderUtil.cpp
visual/opengl/Matrix32Intf.cpp
visual/opengl/Matrix44Intf.cpp
visual/opengl/OffscreenIntf.cpp
visual/opengl/OpenGLPlatformAndroid.cpp
visual/opengl/OpenGLScreen.cpp
visual/opengl/ShaderProgramIntf.cpp
visual/opengl/TextureIntf.cpp
visual/opengl/VertexBinderIntf.cpp
visual/opengl/VertexBufferIntf.cpp )

if( ANDROID_ABI STREQUAL x86 OR ANDROID_ABI STREQUAL x86_64 )
	list(APPEND KRKRZ_SRC 
	sound/MathAlgorithms_SSE.cpp
	sound/RealFFT_SSE.cpp
	sound/WaveFormatConverter_SSE.cpp
	sound/xmmlib.cpp )
endif()

if( ANDROID )
	add_library( krkrz SHARED ${KRKRZ_SRC} )
endif()

# external library
add_subdirectory( external/freetype )
add_subdirectory( external/libjpeg-turbo )
add_subdirectory( external/lpng )
add_subdirectory( external/onig )
add_subdirectory( external/libogg )
add_subdirectory( external/opus )
add_subdirectory( external/opusfile )


if( ANDROID )
	# cpufeature
	include_directories( ${ANDROID_NDK}/sources/android/cpufeatures )
	add_library( cpufeatures ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c )

	# native_app_glue
	# include_directories( ${ANDROID_NDK}/sources/android/native_app_glue )
	# add_library(native_app_glue ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

	# ndk_helper
	#include_directories( ${ANDROID_NDK}/sources/android/ndk_helper ${ANDROID_NDK}/sources/android/native_app_glue )
	#file( GLOB _NDK_HELPER_SRCS ${ANDROID_NDK}/sources/android/ndk_helper/*.cpp ${ANDROID_NDK}/sources/android/ndk_helper/gl3stub.c )
	#add_library( ndk_helper ${_NDK_HELPER_SRCS} )
	include_directories( ${ANDROID_NDK}/sources/android/ndk_helper )
	add_library( ndk_helper ${ANDROID_NDK}/sources/android/ndk_helper/gl3stub.c )

	find_library( log-lib log )
	find_library( android-lib android )
	find_library( EGL-lib EGL )
	find_library( GLESv1_CM-lib GLESv1_CM )
	find_library( GLESv2-lib GLESv2 )
	find_library( z-lib z )
	find_library( opensles OpenSLES )

	# opusfile より opus/oggを後にリンクする必要があることに注意
	target_link_libraries( krkrz

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       ${android-lib}
                       ${EGL-lib}
                       ${GLESv1_CM-lib}
                       ${GLESv2-lib}
                       ${z-lib}
                       ${opensles}
                       atomic
                       
                       # Links my library
                       freetype
                       jpeg-turbo
                       png
                       onig
                       opusfile
                       opus
                       ogg
                       
                       # 
                       cpufeatures
                       ndk_helper
                        )
endif()
endif()

