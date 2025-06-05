import React, { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';

function UserDouches() {
    const { id } = useParams();
    const navigate = useNavigate();
    const [douches, setDouches] = useState([]);
    const [userInfo, setUserInfo] = useState(null);

    useEffect(() => {
        fetch('http://localhost:8080/douches')
            .then(response => response.json())
            .then(data => {
                const filtered = data.filter(d => d.user?.id === parseInt(id));
                setDouches(filtered);

                if (filtered.length > 0) {
                    setUserInfo(filtered[0].user);
                } else {
                    // Si aucune douche, on récupère les infos de l'utilisateur directement
                    fetch(`http://localhost:8080/users/${id}`)
                        .then(res => res.json())
                        .then(user => setUserInfo(user))
                        .catch(err => console.error('Erreur utilisateur', err));
                }
            })
            .catch(error => console.error('Erreur de chargement des douches', error));
    }, [id]);

    return (
        <div>
            <button onClick={() => navigate('/')}>Choisir un autre utilisateur</button>

            {userInfo ? (
                <>
                    <h2>Douches de {userInfo.prenom} {userInfo.nom.toUpperCase()}</h2>

                    {douches.length > 0 ? (
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
                    ) : (
                        <p>Aucune douche enregistrée pour cet utilisateur.</p>
                    )}
                </>
            ) : (
                <p>Chargement de l'utilisateur...</p>
            )}
        </div>
    );
}

export default UserDouches;
