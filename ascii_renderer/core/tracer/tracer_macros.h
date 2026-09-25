#pragma once

#include "trace_builder.h"
#include "severity.h"

#define TRACE_IMPL(sev) core::tracer::TraceBuilder{g_tracer}

#define TRACE_DEBUG() TRACE_IMPL(core::Severity::DEBUG)
#define TRACE_INFO() TRACE_IMPL(core::Severity::INFO)
#define TRACE_WARNING() TRACE_IMPL(core::Severity::WARNING)
#define TRACE_ERROR() TRACE_IMPL(core::Severity::ERROR)