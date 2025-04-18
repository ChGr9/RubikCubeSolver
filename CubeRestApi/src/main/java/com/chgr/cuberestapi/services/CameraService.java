package com.chgr.cuberestapi.services;

import java.io.IOException;
import java.io.InputStream;

public interface CameraService {
    InputStream capture() throws IOException;
}
