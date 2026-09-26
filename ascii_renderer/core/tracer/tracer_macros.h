#pragma once

#include "tracer.h"
#include "trace_builder.h"
#include "severity.h"

#define TRACE_IMPL(sev) \
	if (!Tracer::Get().ShouldTrace(sev)) {} else core::tracer::TraceBuilder{Tracer::Get()}

#define TRACE_DEBUG() TRACE_IMPL(core::Severity::Debug)
#define TRACE_INFO() TRACE_IMPL(core::Severity::Info)
#define TRACE_WARNING() TRACE_IMPL(core::Severity::Warning)
#define TRACE_ERROR() TRACE_IMPL(core::Severity::Error)

#define TRACE_CURRENT_FUNCTION __FUNCTION__
#define TRACE_CURRENT_LINE __LINE__
#define TRACE_CURRENT_FILE __FILE__