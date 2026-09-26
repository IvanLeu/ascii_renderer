#pragma once

#include "tracer.h"
#include "trace_builder.h"
#include "severity.h"

#define TRACE_IMPL(sev) \
	if (!core::tracer::Tracer::Get().ShouldTrace(sev))                         \
    {                                                                          \
    }                                                                          \
    else                                                                       \
        core::tracer::TraceBuilder { core::tracer::Tracer::Get(), sev }

#define TRACE_DEBUG() TRACE_IMPL(core::tracer::Severity::Debug)
#define TRACE_INFO() TRACE_IMPL(core::tracer::Severity::Info)
#define TRACE_WARNING() TRACE_IMPL(core::tracer::Severity::Warning)
#define TRACE_ERROR() TRACE_IMPL(core::tracer::Severity::Error)

#define TRACE_CURRENT_FUNCTION __FUNCTION__
#define TRACE_CURRENT_LINE __LINE__
#define TRACE_CURRENT_FILE __FILE__