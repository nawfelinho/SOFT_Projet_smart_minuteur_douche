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

function UserDouches() {
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

    const formatTemps = (seconds) => {
        const m = Math.floor(seconds / 60);
        const s = seconds % 60;
        return `${m}min ${s}s`;
    };

    const moyenneGlissante = (data, windowSize = 3) => {
        const res = [];
        for (let i = 0; i < data.length; i++) {
            const start = Math.max(0, i - windowSize + 1);
            const window = data.slice(start, i + 1);
            const moyenne = window.reduce((a, b) => a + b, 0) / window.length;
            res.push(moyenne);
        }
        return res;
    };

    const durees = douches.map(d => d.duree);
    const labels = douches.map((_, i) => `Douche ${i + 1}`);
    const moyennes = moyenneGlissante(durees);

    const chartData = {
        labels,
        datasets: [
            {
                label: 'Durée des douches (s)',
                data: durees,
                borderColor: 'deepskyblue',
                backgroundColor: 'deepskyblue',
                fill: false,
                tension: 0.3,
                pointRadius: 4,
                pointHoverRadius: 6,
            },
            {
                label: 'Temps de douche moyen (moyenne glissante)',
                data: moyennes,
                borderColor: 'orange',
                borderDash: [5, 5],
                fill: false,
                tension: 0.3,
                pointRadius: 0,
            },
        ],
    };

    const chartOptions = {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: { position: 'top' },
        },
        scales: {
            y: { beginAtZero: true, title: { display: true, text: 'Durée (s)' } },
        },
    };

    return (
        <div>
            <button onClick={() => navigate('/')}>Choisir un autre utilisateur</button>

            <h1 style={{marginTop: '20px'}}>Douches de {userName}</h1>

            <div style={{marginBottom: '20px'}}>
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

            <h2>📉 Évolution des durées de douche</h2>
            <div style={{ width: '90%', height: '400px', margin: 'auto' }}>
                <Line data={chartData} options={chartOptions} />
            </div>

            <div style={{backgroundColor: '#e8f6fe', padding: '1.5rem', borderRadius: '10px', marginBottom: '2rem'}}>
                <h2 style={{display: 'flex', alignItems: 'center'}}>
                    <span role="img" aria-label="Conseils" style={{marginRight: '0.5rem'}}>💡</span>
                    Conseils utiles
                </h2>
                <ul style={{listStyleType: 'none', paddingLeft: 0, lineHeight: '1.8', fontSize: '1rem'}}>
                    <li>🛁 Une douche de 5 minutes consomme environ 60 litres d’eau.</li>
                    <li>💰 L’eau chaude représente 20% de la consommation énergétique d’un foyer.</li>
                    <li>🌿 Réduire la durée de douche permet d’économiser jusqu’à 270 €/an.</li>
                    <li>🏷️ Un bon objectif est une douche de 5 minutes (300 s).</li>
                    <li>📈 Surveillez votre moyenne pour progresser durablement !</li>
                </ul>
            </div>

            <h2>🔮 Prévisions</h2>

            {douches.length < 5 && (
                <p style={{color: '#666', fontStyle: 'italic', marginBottom: '10px'}}>
                    Faites au moins cinq douches pour débloquer les prévisions.
                </p>
            )}

            <div style={{
                backgroundColor: '#f6f6f6',
                padding: '15px',
                borderRadius: '8px',
                filter: douches.length < 5 ? 'blur(2px)' : 'none',
                position: 'relative'
            }}>
                <span
                    role="img"
                    aria-label="lock"
                    style={{
                        position: 'absolute',
                        top: 10,
                        right: 15,
                        fontSize: '24px',
                        display: douches.length < 5 ? 'inline' : 'none'
                    }}
                >🔒</span>
                <p>Ici seront affichées des prévisions intelligentes sur vos futures douches.</p>
            </div>

            <h2>📊 Données détaillées des douches</h2>
            <table border="1" cellPadding="6">
                <thead>
                <tr>
                    <th>Début</th>
                    <th>Fin</th>
                    <th>Durée (s)</th>
                    <th>Dépassement (s)</th>
                </tr>
                </thead>
                <tbody>
                {douches.map((d, index) => (
                    <tr key={index}>
                        <td>{d.dateDebut}</td>
                        <td>{d.dateFin}</td>
                        <td>{d.duree}</td>
                        <td>{d.tempsDepasse}</td>
                    </tr>
                ))}
                </tbody>
            </table>
        </div>
    );
}

export default UserDouches;
