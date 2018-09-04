{
    "targets": [
        {
            "target_name": "plugin",
            # 'defines': ["EMSCRIPTEN_HAS_UNBOUND_TYPE_NAMES=0"],
            'include_dirs': [
                'include'
            ],
            "sources": ["src/bind.cc",
                        "../example.cc"],
            "conditions": [
                # ["OS==\"mac\"", {"libraries": ["-ljpeg"],
                #                  "cflags": ["-fexceptions", "-Dcimg_display=0", "-Dcimg_use_jpeg"], "cflags_cc": ["-fexceptions", "-Dcimg_display=0", "-Dcimg_use_jpeg"],
                #                  }],
                ["OS==\"linux\"", {"cflags": ["-frtti", "-w"],
                                   "cflags_cc": ["-frtti", "-w"]
                                   }]
                # ["OS==\"win\"", {"libraries": [],
                #                  "cflags": ["-fexceptions", "-Dcimg_display=0"],
                #                  "cflags_cc": ["-fexceptions", "-Dcimg_display=0"]
                #                  }]
            ]
        }
    ]
}
