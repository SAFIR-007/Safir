import sys
import re

def extract_function(file_content, func_name):
    # Matches return types and function name, followed by arguments in parens, then an opening brace.
    # We use re.DOTALL to allow newlines in the arguments if any, but since we just do [^)]*, it's fine.
    pattern = re.compile(r'^\s*(?:inline\s+)?(?:[\w\<\>\[\]]+\s+)+' + func_name + r'\s*\([^)]*\)\s*\{', re.MULTILINE)
    match = pattern.search(file_content)
    if not match:
        return None, file_content
    
    start_idx = match.start()
    
    open_braces = 0
    in_string = False
    in_char = False
    in_comment = False
    in_multiline_comment = False
    
    # We want to find the first '{', which matched in the regex.
    # So we trace back slightly from the end of the match to find the actual '{'
    i = match.end() - 1
    end_idx = -1
    
    while i < len(file_content):
        c = file_content[i]
        
        if in_comment:
            if c == '\n': in_comment = False
            i += 1; continue
        if in_multiline_comment:
            if c == '*' and i+1 < len(file_content) and file_content[i+1] == '/':
                in_multiline_comment = False  # Fixed this in the python script
                i += 2; continue
            i += 1; continue
            
        if in_string:
            if c == '\\': i += 2; continue
            if c == '"': in_string = False
            i += 1; continue
            
        if in_char:
            if c == '\\': i += 2; continue
            if c == "'": in_char = False
            i += 1; continue
            
        if c == '/' and i+1 < len(file_content):
            if file_content[i+1] == '/':
                in_comment = True
                i += 2
                continue
            if file_content[i+1] == '*':
                in_multiline_comment = True
                i += 2
                continue
                
        if c == '"':
            in_string = True
        elif c == "'":
            in_char = True
        elif c == '{':
            open_braces += 1
        elif c == '}':
            open_braces -= 1
            if open_braces == 0:
                end_idx = i + 1
                break
                
        i += 1

    if end_idx != -1:
        func_content = file_content[start_idx:end_idx]
        new_file_content = file_content[:start_idx] + file_content[end_idx:]
        return func_content, new_file_content
    
    return None, file_content

def process():
    with open('iMain.cpp', 'r', encoding='utf-8') as f:
        content = f.read()
        
    mappings = {
        'BounceShooter.h': ['initEnemies', 'drawLevel1'],
        'level1ss.h': ['initNewLevel', 'drawNewLevel', 'updateNewLevel', 'mouseNewLevel', 'keyboardNewLevel', 'keyUpNewLevel', 'specialKeyboardNewLevel', 'specialKeyUpNewLevel'],
        'Puzzle.h': ['initPuzzle', 'drawPuzzleTransition', 'drawSurpriseScreen', 'drawGravityNode', 'drawPuzzle', 'isTilePortOpen', 'isPuzzleAlreadySolved'],
        'Boss1.h': ['p3InitPhase3', 'phase3PlayerInShadow', 'phase3PlayerInSearchlight', 'drawPhase3Arena', 'drawPhase3Boss', 'drawPhase3Player', 'drawPhase3HUD', 'handlePhase3Stealth', 'handlePhase3Combat', 'updatePhase3Player', 'updatePhase3Boss', 'drawIntel', 'drawWarning', 'drawSecondBG', 'drawFinalBG', 'p3IsKeyPressed'],
        'Lab.h': ['initLabLevel', 'drawLabTransition', 'drawLabInstructions', 'drawLabLevel', 'updateLabLevel'],
        'SilentFoundry.h': ['sfGeneratePuzzle', 'sfInit', 'sfPlayerInShadow', 'sfPlayerInVisionCone', 'sfPlayerNearTower', 'sfPlayerInDroneSpotlight', 'sfPlayerInSignalField', 'activateControlPanel', 'deployDecoy', 'sfUpdatePanels', 'sfUpdateDecoys', 'sfUpdatePulse', 'sfUpdateHazards', 'sfUpdateMomentum', 'sfUpdateAdaptiveAI', 'updateSuspicion', 'updateSFDrones', 'updateSFPlayer', 'updateSFBoss', 'checkPuzzleAnswer', 'sfCheckCollisions', 'spawnSFPowerup', 'drawSFArena', 'drawSFBoss', 'drawSFPlayer', 'renderPuzzleUI', 'renderSFHUD', 'sfUpdateEffects', 'drawBossDeadImage']
    }
    
    for header, funcs in mappings.items():
        extracted = []
        for fn in funcs:
            func_code, new_content = extract_function(content, fn)
            if func_code:
                if not func_code.lstrip().startswith('inline'):
                    func_code = 'inline ' + func_code.lstrip()
                extracted.append(func_code)
                content = new_content
            else:
                print(f"Could not find {fn} in iMain.cpp")
                
        if extracted:
            with open(header, 'r', encoding='utf-8') as hf:
                h_content = hf.read()
            if '#endif' in h_content:
                last_endif_idx = h_content.rindex('#endif')
                new_h_content = h_content[:last_endif_idx] + "\n\n" + "\n\n".join(extracted) + "\n\n" + h_content[last_endif_idx:]
                with open(header, 'w', encoding='utf-8') as hf:
                    hf.write(new_h_content)
            else:
                print(f"Warning: No #endif found in {header}. Appending...")
                with open(header, 'a', encoding='utf-8') as hf:
                    hf.write("\n\n" + "\n\n".join(extracted) + "\n\n")

    with open('iMain.cpp', 'w', encoding='utf-8') as f:
        f.write(content)
        
    print("Extraction complete.")

if __name__ == '__main__':
    process()
