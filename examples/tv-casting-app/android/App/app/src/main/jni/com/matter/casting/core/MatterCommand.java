package com.matter.casting.core;

import java.util.concurrent.CompletableFuture;

public abstract class MatterCommand<Request, Response> implements Command<Request, Response> {
    protected long _cppCommand;

    @Override
    public native CompletableFuture<Response> invoke(Request request, Object context, Integer timedInvokeTimeoutMs);
}
