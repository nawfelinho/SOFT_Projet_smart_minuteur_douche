import React, { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import {
    Chart as ChartJS,
    LineElement,
    PointElement,
    LinearScale,
    CategoryScale,
    Legend,
    Tooltip,
} from 'chart.js';
import { Line } from 'react-chartjs-2';

ChartJS.register(LineElement, PointElement, LinearScale, CategoryScale, Legend, Tooltip);

// Constantes d’équivalence pour une douche
const DOUCHES_PAR_AN = 338;
const DUREE_OBJECTIF = 300; // 5 min = 300s

// Valeurs par minute
const LITRES_PAR_MIN = 20;
const KWH_PAR_MIN = 6.972;
const EUROS_PAR_MIN = 0.05;
const CO2_PAR_MIN = 0.942;

function formatTemps(seconds) {
    const m = Math.floor(seconds / 60);
    const s = seconds % 60;
    return `${m}min ${s < 10 ? '0' : ''}${s}s`;
}

function UserPrevisions() {
    const { id } = useParams();
    const navigate = useNavigate();
    const [douches, setDouches] = useState([]);
    const [userName, setUserName] = useState('');
    const [userStats, setUserStats] = useState({ total: 0, moyenne: 0 });

    useEffect(() => {
        fetch(`http://localhost:8080/users/${id}`)
            .then(res => res.json())
            .then(user => setUserName(`${user.prenom} ${user.nom}`));

        fetch('http://localhost:8080/douches')
            .then(res => res.json())
            .then(data => {
                const filtered = data.filter(d => d.user?.id === parseInt(id));
                setDouches(filtered);
                const total = filtered.reduce((sum, d) => sum + d.duree, 0);
                const moyenne = filtered.length ? Math.floor(total / filtered.length) : 0;
                setUserStats({ total, moyenne });
            })
            .catch(err => console.error("Erreur chargement douches", err));
    }, [id]);

    // ---- 1. Objectif intelligent de la prochaine douche ----
    const objectifProchaineDouche = () => {
        if (douches.length === 0) {
            return (
                <div style={{
                    background: "#f8f8f8", borderRadius: "7px", padding: "12px 16px", margin: "22px 0 22px 0"
                }}>
                    <p style={{marginBottom: 4, fontSize: "1rem"}}>
                        <b>Le temps moyen d'une douche recommandé par l’OMS (Organisation mondiale de la santé) est d’environ 5 minutes.</b>
                    </p>
                    <p style={{margin: 0}}>
                        <b>Objectif de temps de la prochaine douche :</b><br />
                        Vous n'avez pas encore démarré de douches, essayez de prendre de bonnes habitudes ! Pour votre première douche, essayez d'atteindre l'objectif suivant : <b>5 min</b>
                    </p>
                </div>
            );
        }
        const moyenne = userStats.moyenne;
        if (moyenne <= DUREE_OBJECTIF) {
            return (
                <div style={{
                    background: "#f8f8f8", borderRadius: "7px", padding: "12px 16px", margin: "22px 0 22px 0"
                }}>
                    <p style={{marginBottom: 4, fontSize: "1rem"}}>
                        <b>Le temps moyen d'une douche recommandé par l’OMS (Organisation mondiale de la santé) est d’environ 5 minutes.</b>
                    </p>
                    <p style={{margin: 0}}>
                        <b>Objectif de temps de la prochaine douche :</b><br />
                        Super, vous avez atteint l'objectif recommandé, gardez vos bonnes habitudes ! Pour la prochaine douche, essayez d'atteindre l'objectif suivant : <b>5 min</b>
                    </p>
                </div>
            );
        }
        // Utilisateur au-dessus de 5 min
        const last3 = douches.slice(-3).map(d => d.duree);
        const moy3 = last3.length ? last3.reduce((a, b) => a + b, 0) / last3.length : moyenne;
        let objReduit = Math.max(moy3 * 0.90, 300); // -10%, au minimum 5min
        // Limite la chute brutale d’un coup
        const last = douches[douches.length - 1].duree;
        if (last - objReduit > 30) objReduit = last - 30;
        objReduit = Math.round(objReduit);

        if (objReduit <= 330) {
            return (
                <div style={{
                    background: "#f8f8f8", borderRadius: "7px", padding: "12px 16px", margin: "22px 0 22px 0"
                }}>
                    <p style={{marginBottom: 4, fontSize: "1rem"}}>
                        <b>Le temps moyen d'une douche recommandé par l’OMS (Organisation mondiale de la santé) est d’environ 5 minutes.</b>
                    </p>
                    <p style={{margin: 0}}>
                        <b>Objectif de temps de la prochaine douche :</b><br />
                        Bravo, vous vous rapprochez de l'objectif ! Essayez d’atteindre ou de maintenir 5 minutes à chaque douche.
                    </p>
                </div>
            );
        }
        return (
            <div style={{
                background: "#f8f8f8", borderRadius: "7px", padding: "12px 16px", margin: "22px 0 22px 0"
            }}>
                <p style={{marginBottom: 4, fontSize: "1rem"}}>
                    <b>Le temps moyen d'une douche recommandé par l’OMS (Organisation mondiale de la santé) est d’environ 5 minutes.</b>
                </p>
                <p style={{margin: 0}}>
                    <b>Objectif de temps de la prochaine douche :</b><br />
                    Vous êtes sur la bonne voie ! Pour votre prochaine douche, essayez d’atteindre l’objectif suivant : <b>{formatTemps(objReduit)}</b>
                </p>
            </div>
        );
    };

    // ---- 2. Prévisions et données pour les graphiques ----
    // Si pas de douches, utiliser 5min comme base pour tout
    const dureeMoyenne = douches.length === 0 ? DUREE_OBJECTIF : userStats.moyenne;

    // Par mois pour la courbe (x12), puis étendre à l’année
    const courbeLongueur = 12;
    // Générer le nombre cumulé de douches par mois (réparties uniformément)
    const douchesParMois = DOUCHES_PAR_AN / 12;

    // Fonctions pour calculer les points d’une courbe cumulative
    const genCourbePrevision = (duree) => {
        // durée en secondes
        const min = duree / 60;
        let litres = 0, kwh = 0, euros = 0, co2 = 0;
        const pointsLitres = [], pointsKwh = [], pointsEuros = [], pointsCO2 = [];
        for (let m = 1; m <= courbeLongueur; m++) {
            litres += min * douchesParMois * LITRES_PAR_MIN;
            kwh += min * douchesParMois * KWH_PAR_MIN;
            euros += min * douchesParMois * EUROS_PAR_MIN;
            co2 += min * douchesParMois * CO2_PAR_MIN;
            pointsLitres.push(Math.round(litres));
            pointsKwh.push(Number(kwh.toFixed(1)));
            pointsEuros.push(Number(euros.toFixed(2)));
            pointsCO2.push(Number(co2.toFixed(2)));
        }
        return { litres: pointsLitres, kwh: pointsKwh, euros: pointsEuros, co2: pointsCO2 };
    };

    const previsionOMS = genCourbePrevision(DUREE_OBJECTIF);
    const previsionUser = genCourbePrevision(dureeMoyenne);

    // Données cumulées sur un an
    const totalOMS = {
        litres: previsionOMS.litres[11],
        kwh: previsionOMS.kwh[11],
        euros: previsionOMS.euros[11],
        co2: previsionOMS.co2[11]
    };
    const totalUser = {
        litres: previsionUser.litres[11],
        kwh: previsionUser.kwh[11],
        euros: previsionUser.euros[11],
        co2: previsionUser.co2[11]
    };
    // Économies possibles
    const gain = {
        litres: Math.max(0, totalUser.litres - totalOMS.litres),
        kwh: Math.max(0, (totalUser.kwh - totalOMS.kwh).toFixed(1)),
        euros: Math.max(0, (totalUser.euros - totalOMS.euros).toFixed(2)),
        co2: Math.max(0, (totalUser.co2 - totalOMS.co2).toFixed(2)),
    };

    // Labels pour les graphiques
    const labels = Array.from({length: courbeLongueur}, (_, i) => `${i+1} mois`);

    // Fonction de rendu générique pour un graphique
    const renderChart = (data1, data2, label, yLabel, legend1, legend2, color1, color2) => (
        <div style={{ width: '90%', height: '330px', margin: '25px auto' }}>
            <Line data={{
                labels,
                datasets: [
                    {
                        label: legend1,
                        data: data1,
                        borderColor: color1,
                        backgroundColor: color1,
                        fill: false,
                        tension: 0.3,
                        borderWidth: 3,
                        pointRadius: 0,
                    },
                    {
                        label: legend2,
                        data: data2,
                        borderColor: color2,
                        backgroundColor: color2,
                        borderDash: [8, 5],
                        fill: false,
                        tension: 0.3,
                        borderWidth: 3,
                        pointRadius: 0,
                    }
                ]
            }} options={{
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: { position: 'top' }
                },
                scales: {
                    y: { beginAtZero: true, title: { display: true, text: yLabel } }
                }
            }}/>
        </div>
    );

    // ---- Section Bilan Annuel (avec logique conditionnelle JS) ----
    const allGainsAreZeroOrNegative =
        gain.litres <= 0 &&
        gain.kwh <= 0 &&
        gain.euros <= 0 &&
        gain.co2 <= 0;

    const bilanAnnuel = (
        <div style={{
            background: "#eaf8f6", borderRadius: "10px", padding: "2rem", marginTop: "40px", marginBottom: "50px"
        }}>
            <h3>📋 Bilan annuel de vos douches</h3>
            <p>
                Avec votre consommation de douches actuelles, vous atteindrez dans un an :
                <br/>🚿 <b>{totalUser.litres}</b> litres d’eau consommés
                <br/>⚡ <b>{totalUser.kwh}</b> kWh d’énergie consommés
                <br/>💶 <b>{totalUser.euros}</b> € de dépense estimée
                <br/>🌍 <b>{totalUser.co2}</b> kgCO₂ émis
            </p>
            {allGainsAreZeroOrNegative ? (
                <p style={{color: "#1a9c32", fontWeight: "bold", marginTop: 20}}>
                    Bravo, votre consommation est déjà inférieure à la recommandation de l’OMS, gardez vos bonnes habitudes !
                </p>
            ) : (
                <p>
                    <b>Par contre, en atteignant l'objectif des 5 min recommandées par l’OMS, vous pourriez économiser :</b>
                    <br/>🚿 <b>{gain.litres}</b> litres d’eau
                    <br/>⚡ <b>{gain.kwh}</b> kWh d’énergie
                    <br/>💶 <b>{gain.euros}</b> €
                    <br/>🌍 <b>{gain.co2}</b> kgCO₂
                </p>
            )}
        </div>
    );

    return (
        <div>
            <button onClick={() => navigate(`/user/${id}`)} style={{marginBottom: '14px'}}>Retour à la page utilisateur</button>

            <h1 style={{marginTop: '10px'}}>Prévisions pour {userName}</h1>
            <div style={{marginBottom: '10px'}}>
                <p style={{margin: '4px 0', fontSize: '16px'}}>
                    <strong>Nombre total de douches :</strong> {douches.length}
                </p>
                <p style={{margin: '4px 0', fontSize: '16px'}}>
                    <strong>Temps total de douches :</strong> {formatTemps(userStats.total)}
                </p>
                <p style={{margin: '4px 0', fontSize: '16px'}}>
                    <strong>Temps moyen par douche :</strong> {formatTemps(userStats.moyenne)}
                </p>
            </div>
            {objectifProchaineDouche()}

            <div style={{marginTop: "36px", marginBottom: "14px", fontSize: "1.15rem"}}>
                <b>Un adulte français prend en moyenne 6 à 7 douches par semaine, soit environ 338 douches par an.</b><br/>
                Voici où vous en serez dans un an avec votre consommation d’eau actuelle liée à vos douches :
            </div>

            {/* 1. Eau */}
            <h2 style={{marginLeft: 10, marginTop: 30, fontSize: "1.27rem"}}>
                💧 Prévision de la consommation d’eau sur un an
            </h2>
            {renderChart(
                previsionUser.litres, previsionOMS.litres,
                "Consommation d'eau (L)", "Litres",
                "Votre consommation d'eau", "Objectif OMS (5 min/douche)",
                "#00bfff", "#34c759"
            )}

            {/* 2. Energie */}
            <h2 style={{marginLeft: 10, marginTop: 30, fontSize: "1.27rem"}}>
                ⚡ Prévision de la consommation d’énergie sur un an
            </h2>
            {renderChart(
                previsionUser.kwh, previsionOMS.kwh,
                "Consommation d'énergie (kWh)", "kWh",
                "Votre consommation d'énergie", "Objectif OMS (5 min/douche)",
                "#ff9900", "#009ee0"
            )}

            {/* 3. Coût */}
            <h2 style={{marginLeft: 10, marginTop: 30, fontSize: "1.27rem"}}>
                💶 Prévision du coût des douches sur un an
            </h2>
            {renderChart(
                previsionUser.euros, previsionOMS.euros,
                "Coût (€)", "Coût (€)",
                "Votre coût", "Objectif OMS (5 min/douche)",
                "#db2777", "#10b981"
            )}

            {/* 4. CO2 */}
            <h2 style={{marginLeft: 10, marginTop: 30, fontSize: "1.27rem"}}>
                🌍 Prévision de l’empreinte carbone sur un an
            </h2>
            {renderChart(
                previsionUser.co2, previsionOMS.co2,
                "Empreinte carbone (kgCO₂)", "kgCO₂",
                "Votre empreinte carbone", "Objectif OMS (5 min/douche)",
                "#8c52ff", "#47b247"
            )}

            {bilanAnnuel}
        </div>
    );
}

export default UserPrevisions;
