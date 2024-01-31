package com.matter.casting.core;

import java.util.concurrent.CompletableFuture;

public interface Command<Request, Response> {
    CompletableFuture<Response> invoke(Request request, Object context, Integer timedInvokeTimeoutMs);
}
