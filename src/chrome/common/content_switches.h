// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Defines all the "content" command-line switches.

#ifndef CONTENT_PUBLIC_COMMON_CONTENT_SWITCHES_H_
#define CONTENT_PUBLIC_COMMON_CONTENT_SWITCHES_H_

#include "build/build_config.h"
//#include "chrome/content/content_export.h"

namespace switches {

// All switches in alphabetical order. The switches should be documented
// alongside the definition of their values in the .cc file.
extern const char kAcceleratedCanvas2dMSAASampleCount[];
extern const char kAllowFileAccessFromFiles[];
extern const char kAllowLoopbackInPeerConnection[];
extern const char kAllowNoSandboxJob[];
extern const char kAllowSandboxDebugging[];
extern const char kAuditAllHandles[];
extern const char kAuditHandles[];
extern const char kBlinkPlatformLogChannels[];
extern const char kBlockCrossSiteDocuments[];
extern const char kBrowserCrashTest[];
extern const char kBrowserSubprocessPath[];
extern const char kDebugPluginLoading[];
extern const char kDefaultTileWidth[];
extern const char kDefaultTileHeight[];
extern const char kDisable2dCanvasAntialiasing[];
extern const char kDisable3DAPIs[];
extern const char kDisableBlinkFeatures[];
extern const char kDisableAccelerated2dCanvas[];
extern const char kDisableAcceleratedJpegDecoding[];
extern const char kDisableAcceleratedVideoDecode[];
extern const char kDisableBackingStoreLimit[];
extern const char kDisableBlinkScheduler[];
extern const char kDisablePreferCompositingToLCDText[];
extern const char kDisableDatabases[];
extern const char kDisableDelegatedRenderer[];
extern const char kDisableDirectNPAPIRequests[];
extern const char kDisableDistanceFieldText[];
extern const char kDisableDisplayList2dCanvas[];
extern const char kDisableDomainBlockingFor3DAPIs[];
extern const char kDisableEncryptedMedia[];
extern const char kDisableExperimentalWebGL[];
extern const char kDisableFileSystem[];
extern const char kDisableFlash3d[];
extern const char kDisableFlashStage3d[];
extern const char kDisableGpu[];
extern const char kDisableGpuCompositing[];
extern const char kDisableGpuEarlyInit[];
extern const char kDisableGpuProcessCrashLimit[];
extern const char kDisableGpuRasterization[];
extern const char kDisableGpuSandbox[];
extern const char kDisableGpuWatchdog[];
extern const char kDisableLowResTiling[];
extern const char kDisableHangMonitor[];
extern const char kDisableHideInactiveStackedTabCloseButtons[];
extern const char kDisableHistogramCustomizer[];
extern const char kDisableImplSidePainting[];
extern const char kDisableJava[];
extern const char kDisableLCDText[];
extern const char kDisablePrefixedEncryptedMedia[];
extern const char kDisableKillAfterBadIPC[];
extern const char kDisableLocalStorage[];
extern const char kDisableLogging[];
extern const char kDisableMediaSource[];
extern const char kDisableNamespaceSandbox[];
extern const char kDisableNotifications[];
extern const char kDisableOneCopy[];
extern const char kDisablePepper3d[];
extern const char kDisablePinch[];
extern const char kDisablePermissionsAPI[];
extern const char kDisablePluginsDiscovery[];
extern const char kDisableReadingFromCanvas[];
extern const char kDisableRemoteFonts[];
extern const char kDisableRendererAccessibility[];
extern const char kDisableSeccompFilterSandbox[];
extern const char kDisableSetuidSandbox[];
extern const char kDisableSharedWorkers[];
extern const char kDisableSingleThreadProxyScheduler[];
extern const char kDisableSmoothScrolling[];
extern const char kDisableSoftwareRasterizer[];
extern const char kDisableSVG1DOM[];
extern const char kDisableTextBlobs[];
extern const char kDisableThreadedCompositing[];
extern const char kDisableThreadedScrolling[];
extern const char kDisableV8IdleTasks[];
extern const char kDisableWebSecurity[];
extern const char kDisableXSSAuditor[];
extern const char kDomAutomationController[];
extern const char kEnable2dCanvasClipAntialiasing[];
extern const char kEnableAggressiveDOMStorageFlushing[];
extern const char kEnableBleedingEdgeRenderingFastPaths[];
extern const char kEnableCompositorAnimationTimelines[];
extern const char kEnableCredentialManagerAPI[];
extern const char kEnableBeginFrameScheduling[];
extern const char kEnablePreferCompositingToLCDText[];
extern const char kEnableBlinkFeatures[];
extern const char kEnableBrowserSideNavigation[];
extern const char kEnableDeferredImageDecoding[];
extern const char kEnableDelayAgnosticAec[];
extern const char kEnableDelegatedRenderer[];
extern const char kEnableDisplayList2dCanvas[];
extern const char kEnableDistanceFieldText[];
extern const char kEnableDownloadResumption[];
extern const char kEnableExperimentalCanvasFeatures[];
extern const char kEnableExperimentalWebPlatformFeatures[];
extern const char kEnableGpuClientTracing[];
extern const char kEnableGpuRasterization[];
extern const char kGpuRasterizationMSAASampleCount[];
extern const char kEnableLowResTiling[];
extern const char kEnableImageColorProfiles[];
extern const char kEnableLCDText[];
extern const char kEnableLogging[];
extern const char kEnableMemoryBenchmarking[];
extern const char kEnableNetworkInformation[];
extern const char kEnableOneCopy[];
extern const char kEnableOverlayFullscreenVideo[];
extern const char kEnablePinch[];
extern const char kEnablePreciseMemoryInfo[];
extern const char kEnablePushMessagePayload[];
extern const char kEnablePushMessagingHasPermission[];
extern const char kEnableRegionBasedColumns[];
extern const char kEnableSandboxLogging[];
extern const char kEnableSeccompFilterSandbox[];
extern const char kEnableSkiaBenchmarking[];
extern const char kEnableSlimmingPaint[];
extern const char kEnableSmoothScrolling[];
extern const char kEnableSpatialNavigation[];
extern const char kEnableStaleWhileRevalidate[];
extern const char kEnableStatsTable[];
extern const char kEnableStrictMixedContentChecking[];
extern const char kEnableStrictPowerfulFeatureRestrictions[];
extern const char kEnableStrictSiteIsolation[];
extern const char kEnableServiceWorkerSync[];
extern const char kEnableTcpFastOpen[];
extern const char kEnableThreadedCompositing[];
extern const char kEnableTracing[];
extern const char kEnableTracingOutput[];
extern const char kEnableUserMediaScreenCapturing[];
extern const char kEnableViewport[];
extern const char kEnableViewportMeta[];
extern const char kMainFrameResizesAreOrientationChanges[];
extern const char kEnableVtune[];
extern const char kEnableWebGLDraftExtensions[];
extern const char kEnableWebGLImageChromium[];
extern const char kEnableZeroCopy[];
extern const char kExtraPluginDir[];
extern const char kForceDisplayList2dCanvas[];
extern const char kForceFieldTrials[];
extern const char kForceGpuRasterization[];
extern const char kForceRendererAccessibility[];
extern const char kForceTextBlobs[];
extern const char kGpuDeviceID[];
extern const char kGpuDriverVendor[];
extern const char kGpuDriverVersion[];
extern const char kGpuLauncher[];
extern const char kGpuProcess[];
extern const char kGpuSandboxAllowSysVShm[];
extern const char kGpuSandboxFailuresFatal[];
extern const char kGpuSandboxStartEarly[];
extern const char kGpuStartupDialog[];
extern const char kGpuVendorID[];
extern const char kHostResolverRules[];
extern const char kIgnoreCertificateErrors[];
extern const char kIgnoreGpuBlacklist[];
extern const char kInProcessGPU[];
extern const char kIPCConnectionTimeout[];
extern const char kJavaScriptFlags[];
extern const char kLoadPlugin[];
extern const char kLogGpuControlListDecisions[];
extern const char kLoggingLevel[];
extern const char kLogNetLog[];
extern const char kLogPluginMessages[];
extern const char kMaxUntiledLayerHeight[];
extern const char kMaxUntiledLayerWidth[];
extern const char kMemoryMetrics[];
extern const char kMuteAudio[];
extern const char kNoReferrers[];
extern const char kNoSandbox[];
extern const char kNumRasterThreads[];
extern const char kOverscrollHistoryNavigation[];
extern const char kPluginLauncher[];
extern const char kPluginPath[];
extern const char kPluginProcess[];
extern const char kPluginStartupDialog[];
extern const char kPpapiBrokerProcess[];
extern const char kPpapiFlashArgs[];
extern const char kPpapiInProcess[];
extern const char kPpapiPluginLauncher[];
extern const char kPpapiPluginProcess[];
extern const char kPpapiStartupDialog[];
extern const char kProcessPerSite[];
extern const char kProcessPerTab[];
extern const char kProcessType[];
extern const char kReduceSecurityForTesting[];
extern const char kReducedReferrerGranularity[];
extern const char kRegisterPepperPlugins[];
extern const char kRemoteDebuggingPort[];
extern const char kRendererCmdPrefix[];
extern const char kRendererProcess[];
extern const char kRendererProcessLimit[];
extern const char kRendererStartupDialog[];
extern const char kRootLayerScrolls[];
extern const char kSandboxIPCProcess[];
extern const char kScrollEndEffect[];
extern const char kShowPaintRects[];
extern const char kSingleProcess[];
extern const char kSitePerProcess[];
extern const char kSkipGpuDataLoading[];
extern const char kStartFullscreen[];
extern const char kStatsCollectionController[];
extern const char kTabCaptureDownscaleQuality[];
extern const char kTabCaptureUpscaleQuality[];
extern const char kTestingFixedHttpPort[];
extern const char kTestingFixedHttpsPort[];
extern const char kTestType[];
extern const char kTraceShutdown[];
extern const char kTraceShutdownFile[];
extern const char kTraceStartup[];
extern const char kTraceStartupDuration[];
extern const char kTraceStartupFile[];
extern const char kTraceUploadURL[];
extern const char kUIPrioritizeInGpuProcess[];
extern const char kUseFakeUIForMediaStream[];
extern const char kEnableNativeGpuMemoryBuffers[];
extern const char kUseImageTextureTarget[];
extern const char kUseMobileUserAgent[];
extern const char kUseNormalPriorityForTileTaskWorkerThreads[];
extern const char kUseSurfaces[];
extern const char kDisableSurfaces[];
extern const char kUtilityCmdPrefix[];
extern const char kUtilityProcess[];
extern const char kUtilityProcessAllowedDir[];
extern const char kUtilityProcessEnableMDns[];
extern const char kUtilityProcessRunningElevated[];
extern const char kV8CacheOptions[];
extern const char kValidateInputEventStream[];
extern const char kWaitForDebuggerChildren[];
extern const char kZygoteCmdPrefix[];
extern const char kZygoteProcess[];

#if defined(ENABLE_WEBRTC)
extern const char kDisableWebRtcHWDecoding[];
extern const char kDisableWebRtcEncryption[];
extern const char kDisableWebRtcHWEncoding[];
extern const char kEnableWebRtcHWH264Encoding[];
extern const char kEnableWebRtcStunOrigin[];
extern const char kWebRtcMaxCaptureFramerate[];
#if defined(OS_LINUX) || defined(OS_MACOSX)
extern const char kEnableWebRtcCaptureToTexture[];
#endif
#endif

#if defined(OS_ANDROID)
extern const char kDisableGestureRequirementForMediaPlayback[];
extern const char kDisableClickDelay[];
extern const char kDisableOverscrollEdgeEffect[];
extern const char kDisablePullToRefreshEffect[];
extern const char kDisableScreenOrientationLock[];
extern const char kDisableWebRTC[];
extern const char kHideScrollbars[];
extern const char kNetworkCountryIso[];
extern const char kRemoteDebuggingSocketName[];
extern const char kRendererWaitForJavaDebugger[];
#endif

extern const char kDisableWebAudio[];

#if defined(OS_CHROMEOS)
extern const char kDisablePanelFitting[];
extern const char kDisableVaapiAcceleratedVideoEncode[];
#endif

#if defined(OS_LINUX) && !defined(OS_CHROMEOS)
extern const char kEnableSpeechDispatcher[];
#endif

#if defined(OS_MACOSX) && !defined(OS_IOS)
extern const char kDisableCoreAnimationPlugins[];
extern const char kDisableThreadedEventHandlingMac[];
#endif

#if defined(OS_WIN)
// This switch contains the device scale factor passed to certain processes
// like renderers, etc.
extern const char kDeviceScaleFactor[];
extern const char kDisableLegacyIntermediateWindow[];
// This switch will be removed when we enable the win32K lockdown process
// mitigation.
extern const char kDisableWin32kRendererLockDown[];
extern const char kEnableWin32kRendererLockDown[];
// Switch to uniquely identify names shared memory section for font cache
// across chromium flavors.
extern const char kFontCacheSharedMemSuffix[];
#endif

extern const char kEnableNpapi[];

// DON'T ADD RANDOM STUFF HERE. Put it in the main section above in
// alphabetical order, or in one of the ifdefs (also in order in each section).

}  // namespace switches

#endif  // CONTENT_PUBLIC_COMMON_CONTENT_SWITCHES_H_
