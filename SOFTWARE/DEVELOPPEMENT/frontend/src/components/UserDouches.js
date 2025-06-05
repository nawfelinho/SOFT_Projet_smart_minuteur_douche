import React, { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';

function UserDouches() {
    const { id } = useParams();
    const navigate = useNavigate();
    const [douches, setDouches] = useState([]);

    useEffect(() => {
        fetch('http://localhost:8080/douches')
            .then(response => response.json())
            .then(data => {
                const filtered = data.filter(d => d.user.id === parseInt(id));
                setDouches(filtered);
            })
            .catch(error => console.error('Erreur de chargement des douches', error));
    }, [id]);

    return (
        <div>
            <button onClick={() => navigate('/')}>Choisir un autre utilisateur</button>
            <h2>Douches de l'utilisateur {id}</h2>
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
