{
    "targets": [
        {
            "target_name": "wa-n-api",
            'defines': ["EMSCRIPTEN_HAS_UNBOUND_TYPE_NAMES=0"],
            'include_dirs': [
                'include',
            ],
            "sources": ["src/bind.cc",
                        "src/../example.cc"]
        }
    ]
}
