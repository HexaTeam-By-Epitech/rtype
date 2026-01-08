#!/usr/bin/env python3
"""
Rapport de couverture de code pour les fichiers serveur uniquement
"""
import subprocess
import os

os.chdir('build/linux-debug')

# Liste tous les .gcda des fichiers serveur
gcda_files = []
for root, dirs, files in os.walk('server/CMakeFiles/rtype_server_lib.dir'):
    for f in files:
        if f.endswith('.cpp.gcda'):
            gcda_files.append(os.path.join(root, f))

coverage_data = {}
total_lines = 0
covered_lines = 0

# Analyse chaque fichier
for gcda in gcda_files:
    dir_path = os.path.dirname(gcda)
    result = subprocess.run(['gcov', '-o', dir_path, gcda], 
                          capture_output=True, text=True)
    
    current_file = None
    for line in result.stdout.split('\n'):
        if line.startswith("File '") and '/server/' in line and line.endswith(".cpp'"):
            current_file = line.split("'")[1]
            current_file = os.path.basename(current_file)
        elif 'Lines executed:' in line and current_file:
            # Format: "Lines executed:XX.XX% of YYY"
            parts = line.replace(':', ' ').replace('%', ' ').split()
            percent = float(parts[2])
            total = int(parts[4])
            covered = int(total * percent / 100)
            
            coverage_data[current_file] = {
                'percent': percent,
                'covered': covered,
                'total': total
            }
            
            total_lines += total
            covered_lines += covered
            current_file = None

# Affichage du rapport
print("=" * 90)
print("COUVERTURE CODE SERVEUR (fichiers .cpp dans /server/ uniquement)")
print("=" * 90)
print()

# Tri par pourcentage décroissant
sorted_files = sorted(coverage_data.items(), key=lambda x: x[1]['percent'], reverse=True)

for filename, data in sorted_files:
    pct = data['percent']
    cov = data['covered']
    tot = data['total']
    
    # Barre de progression
    bar_length = int(pct / 2)
    bar = '█' * bar_length + '░' * (50 - bar_length)
    
    # Statut
    if pct >= 80:
        status = "✅"
    elif pct >= 50:
        status = "⚠️ "
    else:
        status = "❌"
    
    print(f"{status} {filename:40s} {bar} {pct:5.1f}% ({cov:3d}/{tot:3d})")

# Statistiques globales
if total_lines > 0:
    overall = (covered_lines / total_lines) * 100
    print()
    print("=" * 90)
    print(f"TOTAL: {covered_lines}/{total_lines} lignes couvertes")
    print(f"COUVERTURE GLOBALE SERVEUR: {overall:.2f}%")
    print("=" * 90)
    print()
    
    if overall >= 80:
        print("✅ OBJECTIF 80% ATTEINT! 🎉🎉🎉")
    else:
        gap = 80 - overall
        needed = int(0.80 * total_lines - covered_lines)
        print(f"⚠️  Il manque {gap:.1f}% pour atteindre 80%")
        print(f"   Environ {needed} lignes supplémentaires à couvrir")
        print()
        print("Fichiers prioritaires à améliorer (par impact potentiel):")
        
        # Calcul impact: lignes non couvertes * écart à 80%
        priority = []
        for filename, data in coverage_data.items():
            if data['percent'] < 80:
                uncovered = data['total'] - data['covered']
                gap_to_80 = 80 - data['percent']
                impact = uncovered * gap_to_80 / 100
                priority.append((filename, data['percent'], data['total'], uncovered, impact))
        
        priority.sort(key=lambda x: x[4], reverse=True)
        
        for i, (fname, pct, tot, uncov, impact) in enumerate(priority[:5], 1):
            target_cov = int(tot * 0.80)
            gain = target_cov - (tot - uncov)
            print(f"  {i}. {fname:35s} {pct:5.1f}% → 80% = +{gain:3d} lignes (impact: {impact:.0f})")
