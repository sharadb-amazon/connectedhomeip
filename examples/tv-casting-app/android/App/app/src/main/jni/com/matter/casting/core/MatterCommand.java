package com.matter.casting.core;

public abstract class MatterCommand<Request, Response> implements Command<Request, Response> {
    protected long _cppCommand;
}
