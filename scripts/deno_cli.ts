import { WASMEnv } from "./wasmenv.ts";

export async function loadWasmEnv(source: string) {
    const encoder = new TextEncoder();
    const module = new WASMEnv();

    let instance;

    // File path: read bytes directly. Deno.readFile handles URL/path.
    // @ts-ignore
    const bytes = await Deno.readFile(source);

    // Try streaming compile first; it's faster for larger modules.
    // compileStreaming needs a Response, so wrap the bytes.
    const stream = new Blob([ bytes ]).stream();
    const response = new Response(stream, {
        headers: { 'Content-Type': 'application/wasm' },
    });
    await module.compileModuleStreaming(response);
    return module;
}

/* ------------------------------------------------------------------ */
/*  test                                                            */
/* ------------------------------------------------------------------ */
const scripts = `
(do
    (= read_file (fn (file_path)
        (let vfp (vfopen file_path))
        (let text "")
        (let chr (vfgetc vfp))
        (while (> chr 0)
            (strappend text chr)
            (= chr (vfgetc vfp))
        )
        text
    ))

    (setstr "result" "你好世界")
    (setnum "free" (memfree))
    (let retext (read_file "main.fe"))
    (print "File Content:")
    (print retext)
    (let x (cons 2 3))
    (setcar x 999)
    (print "x =" x)
)
`;
// @ts-ignore
if (import.meta.main) {
    const module = await loadWasmEnv("build/card_maker_plugin_wren.wasm");
    module.addEventListener("outputLine", (event) => {
        console.log(event.data.lastLine);
    });
    module.addEventListener("variableChanged", (event) => {
        console.log("> variable changed:", event.data.name, "=", event.data.value, "<");
    });
    await module.initInstance();
    const exp = module.getExportedFunctions<{ eval_vfile?: (readerId: number) => void; }>();
    // write file
    const encoder = new TextEncoder();
    module.filesystem.writeFile("main.fe", encoder.encode(scripts));
    const readerId = module.filesystem.openFileReader("main.fe");
    exp.eval_vfile?.(readerId);
}